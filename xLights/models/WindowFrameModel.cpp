/***************************************************************
 * This source files comes from the xLights project
 * https://www.xlights.org
 * https://github.com/smeighan/xLights
 * See the github commit history for a record of contributing
 * developers.
 * Copyright claimed based on commit dates recorded in Github
 * License: https://github.com/smeighan/xLights/blob/master/License.txt
 **************************************************************/

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/xml/xml.h>

#include "WindowFrameModel.h"
#include "ModelScreenLocation.h"

WindowFrameModel::WindowFrameModel(wxXmlNode *node, const ModelManager &manager, bool zeroBased) : ModelWithScreenLocation(manager)
{
    rotation = (node->GetAttribute("Rotation", "CW") == "Clockwise" || node->GetAttribute("Rotation", "CW") == "CW") ? 0 : 1;
    SetFromXml(node, zeroBased);
}

WindowFrameModel::~WindowFrameModel()
{
    //dtor
}
 
void WindowFrameModel::InitModel() {
    rotation = (ModelXml->GetAttribute("Rotation", "CW") == "Clockwise" || ModelXml->GetAttribute("Rotation", "CW") == "CW") ? 0 : 1;
    InitFrame();
    screenLocation.RenderDp = 10.0f;  // give the bounding box a little depth
}

int WindowFrameModel::NodesPerString() const
{
    int ts = GetSmartTs();
    if (ts <= 1) {
        return GetChanCount() / std::max(GetChanCountPerNode(), 1);
    }
    else {
        return ts * (GetChanCount() / std::max(GetChanCountPerNode(), 1));
    }
}

void WindowFrameModel::GetCoordinates(int side, bool clockwise, bool LtoR, bool TtoB, float& x, float& y, float& screenx, float& screeny)
{
    // sides - left, top, right, bottom

    float left = parm2;
    float top = parm1;
    float bottom = parm3;

    float width = std::max(top, bottom) + 2;
    float height = parm2;

    int lengths[] = { (int)left, (int)top, (int)left, (int)bottom };

    if (LtoR) {
        if (TtoB) {
            y = height - 1;
            screenx = -width / 2;
            screeny = height / 2;
            if (clockwise) {
                x = 1;
            }
            else {
                x = 0;
            }
        } 
        else {
            y = 0;
            screenx = -width / 2;
            screeny = -height / 2;
            if (clockwise) {
                x = 0;
            }
            else {
                x = 1;
            }
        }
    }
    else {
        if (TtoB) {
            y = height - 1;
            screenx = width / 2;
            screeny = height / 2;
            if (clockwise) {
                x = width;
            }
            else {
                x = width - 1;
            }
        }
        else {
            y = 0;
            screenx = width / 2;
            screeny = -height / 2;
            if (clockwise) {
                x = width -1;
            }
            else {
                x = width;
            }
        }
    }
}

// initialize buffer coordinates
// parm1=Nodes on Top
// parm2=Nodes left and right
// parm3=Nodes on Bottom
void WindowFrameModel::InitFrame()
{
    SetNodeCount(1, parm1 + 2 * parm2 + parm3, rgbOrder);

    int left = parm2;
    int top = parm1;
    int bottom = parm3;

    int width = std::max(top, bottom) + 2;
    int height = parm2;

    SetBufferSize(height, width);   // treat as outside of matrix
    screenLocation.SetRenderSize(width, height);

    if (left + top + bottom == 0) return;

    int chan = stringStartChan[0];
    int ChanIncr = GetNodeChannelCount(StringType);

    float dir = (ModelXml->GetAttribute("Rotation", "CW") == "Clockwise" || ModelXml->GetAttribute("Rotation", "CW") == "CW") ? 1.0 : -1.0;

    float top_screenincr = (float)(width) / (float)(top + 1.0);
    float bot_screenincr = -1.0 * (float)(width) / (float)(bottom + 1.0);
    float top_incr = (float)(width-1) / (float)(top + 1.0);
    float bot_incr = -1.0 * (float)(width-1) / (float)(bottom + 1.0);
    wxASSERT(top_incr >= 1.0);
    wxASSERT(bot_incr <= -1.0);

    int lengths[] = { left, top, left, bottom };
    float xscreenincr[] = { 0, top_screenincr, 0, bot_screenincr };
    float yscreenincr[] = { 1, 0, -1, 0 };
    float xincr[] = { 0, top_incr, 0, bot_incr };
    float yincr[] = { 1, 0, -1, 0 };
    float xStart[] = { 0, 1, (float)width - 1, (float)width - 2 };
    float yStart[] = { 0, (float)height - 1, (float)height - 1, 0 };
    float xScreenStart[] = { -(float)width / 2, -(float)width / 2 + top_screenincr, (float)width / 2, (float)width / 2 + bot_screenincr };
    float yScreenStart[] = { -(float)(height-1) / 2, (float)(height-1) / 2, (float)(height-1) / 2, -(float)(height-1) / 2 };

    int indexes[] = { 0, 1, 2, 3 };

    if (dir == -1) {
        xStart[0] = 0;
        xStart[1] = (float)width - 2;
        xStart[2] = (float)width - 1;
        xStart[3] = 1;
        yStart[0] = (float)height - 1;
        yStart[1] = (float)height - 1;
        yStart[2] = 0;
        yStart[3] = 0;
        xScreenStart[0] = -(float)width / 2;
        xScreenStart[1] = (float)width / 2 - top_screenincr;
        xScreenStart[2] = (float)width / 2;
        xScreenStart[3] = -(float)width / 2 - bot_screenincr;
        yScreenStart[0] = (float)(height - 1) / 2;
        yScreenStart[1] = (float)(height - 1) / 2;
        yScreenStart[2] = -(float)(height - 1) / 2;
        yScreenStart[3] = -(float)(height - 1) / 2;
    }

    if (IsLtoR) {
        // L to R
        if (isBotToTop) {
            // Bottom left
            if (dir == 1.0) {
                // CW
                indexes[0] = 0;
                indexes[1] = 1;
                indexes[2] = 2;
                indexes[3] = 3;
            }
            else {
                // CCW
                indexes[0] = 3;
                indexes[1] = 2;
                indexes[2] = 1;
                indexes[3] = 0;
            }
        }
        else {
            // Top left
            if (dir == 1.0) {
                // CW
                indexes[0] = 1;
                indexes[1] = 2;
                indexes[2] = 3;
                indexes[3] = 0;
            }
            else {
                // CCW
                indexes[0] = 0;
                indexes[1] = 3;
                indexes[2] = 2;
                indexes[3] = 1;
            }
        }
    }
    else {
        // R to L
        if (isBotToTop) {
            // Bottom right
            if (dir == 1.0) {
                // CW
                indexes[0] = 3;
                indexes[1] = 0;
                indexes[2] = 1;
                indexes[3] = 2;
            }
            else {
                // CCW
                indexes[0] = 2;
                indexes[1] = 1;
                indexes[2] = 0;
                indexes[3] = 3;
            }
        }
        else {
            // Top right
            if (dir == 1.0) {
                // CW
                indexes[0] = 2;
                indexes[1] = 3;
                indexes[2] = 0;
                indexes[3] = 1;
            }
            else {
                // CCW
                indexes[0] = 1;
                indexes[1] = 0;
                indexes[2] = 3;
                indexes[3] = 2;
            }
        }
    }

    int side = 0;
    while (lengths[indexes[side]] == 0) side++;

    float x = xStart[indexes[side]];
    float y = yStart[indexes[side]];
    float screenx = xScreenStart[indexes[side]];
    float screeny = yScreenStart[indexes[side]];
    int curLen = lengths[indexes[side]];

    size_t NodeCount = GetNodeCount();
    for (size_t n = 0; n < NodeCount; n++) {
        wxASSERT(curLen > 0);
        Nodes[n]->ActChan = chan;
        chan += ChanIncr;
        size_t coordCount = GetCoordCount(n);
        wxASSERT(coordCount == 1); // only one coord supported by this code
        for (size_t c = 0; c < coordCount; c++) {
            Nodes[n]->Coords[c].bufX = x;
            Nodes[n]->Coords[c].bufY = y;
            Nodes[n]->Coords[c].screenX = screenx;
            Nodes[n]->Coords[c].screenY = screeny;
        }
        screenx = screenx + (xscreenincr[indexes[side]] * dir);
        screeny = screeny + (yscreenincr[indexes[side]] * dir);
        x = x + xincr[indexes[side]] * dir;
        y = y + yincr[indexes[side]] * dir;
        curLen--;
        if (curLen <= 0) {
            side++;
            if (side >= 4) side = 0;
            while (lengths[indexes[side]] == 0) {
                side++;
                if (side >= 4) side = 0;
            }
            x = xStart[indexes[side]];
            y = yStart[indexes[side]];
            screenx = xScreenStart[indexes[side]];
            screeny = yScreenStart[indexes[side]];
            curLen = lengths[indexes[side]];
        }
    }
}

static const char* TOP_BOT_LEFT_RIGHT_VALUES[] = { 
        "Top Left",
        "Top Right",
        "Bottom Left",
        "Bottom Right"
};
static wxPGChoices TOP_BOT_LEFT_RIGHT(wxArrayString(4, TOP_BOT_LEFT_RIGHT_VALUES));

static const char* CLOCKWISE_ANTI_VALUES[] = {
        "Clockwise",
        "Counter Clockwise"
};
static wxPGChoices CLOCKWISE_ANTI(wxArrayString(2, CLOCKWISE_ANTI_VALUES));

void WindowFrameModel::AddTypeProperties(wxPropertyGridInterface *grid) {
    wxPGProperty *p = grid->Append(new wxUIntProperty("# Lights Top", "WFTopCount", parm1));
    p->SetAttribute("Min", 0);
    p->SetAttribute("Max", 1000);
    p->SetEditor("SpinCtrl");

    p = grid->Append(new wxUIntProperty("# Lights Left/Right", "WFLeftRightCount", parm2));
    p->SetAttribute("Min", 0);
    p->SetAttribute("Max", 1000);
    p->SetEditor("SpinCtrl");

    p = grid->Append(new wxUIntProperty("# Lights Bottom", "WFBottomCount", parm3));
    p->SetAttribute("Min", 0);
    p->SetAttribute("Max", 1000);
    p->SetEditor("SpinCtrl");

    grid->Append(new wxEnumProperty("Starting Location", "WFStartLocation", TOP_BOT_LEFT_RIGHT, IsLtoR ? (isBotToTop ? 2 : 0) : (isBotToTop ? 3 : 1)));

    grid->Append(new wxEnumProperty("Direction", "WFDirection", CLOCKWISE_ANTI, rotation));
}

int WindowFrameModel::OnPropertyGridChange(wxPropertyGridInterface *grid, wxPropertyGridEvent& event) {
    if ("WFTopCount" == event.GetPropertyName()) {
        ModelXml->DeleteAttribute("parm1");
        ModelXml->AddAttribute("parm1", wxString::Format("%d", (int)event.GetPropertyValue().GetLong()));
        AddASAPWork(OutputModelManager::WORK_RGBEFFECTS_CHANGE, "WindowFrameModel::OnPropertyGridChange::WFTopCount");
        AddASAPWork(OutputModelManager::WORK_MODELS_CHANGE_REQUIRING_RERENDER, "WindowFrameModel::OnPropertyGridChange::WFTopCount");
        AddASAPWork(OutputModelManager::WORK_RELOAD_MODEL_FROM_XML, "WindowFrameModel::OnPropertyGridChange::WFTopCount");
        AddASAPWork(OutputModelManager::WORK_REDRAW_LAYOUTPREVIEW, "WindowFrameModel::OnPropertyGridChange::WFTopCount");
        AddASAPWork(OutputModelManager::WORK_RELOAD_MODELLIST, "WindowFrameModel::OnPropertyGridChange::WFTopCount");
        return 0;
    } else if ("WFLeftRightCount" == event.GetPropertyName()) {
        ModelXml->DeleteAttribute("parm2");
        ModelXml->AddAttribute("parm2", wxString::Format("%d", (int)event.GetPropertyValue().GetLong()));
        AddASAPWork(OutputModelManager::WORK_RGBEFFECTS_CHANGE, "WindowFrameModel::OnPropertyGridChange::WFLeftRightCount");
        AddASAPWork(OutputModelManager::WORK_MODELS_CHANGE_REQUIRING_RERENDER, "WindowFrameModel::OnPropertyGridChange::WFLeftRightCount");
        AddASAPWork(OutputModelManager::WORK_RELOAD_MODEL_FROM_XML, "WindowFrameModel::OnPropertyGridChange::WFLeftRightCount");
        AddASAPWork(OutputModelManager::WORK_REDRAW_LAYOUTPREVIEW, "WindowFrameModel::OnPropertyGridChange::WFLeftRightCount");
        AddASAPWork(OutputModelManager::WORK_RELOAD_MODELLIST, "WindowFrameModel::OnPropertyGridChange::WFLeftRightCount");
        return 0;
    } else if ("WFBottomCount" == event.GetPropertyName()) {
        ModelXml->DeleteAttribute("parm3");
        ModelXml->AddAttribute("parm3", wxString::Format("%d", (int)event.GetPropertyValue().GetLong()));
        AddASAPWork(OutputModelManager::WORK_RGBEFFECTS_CHANGE, "WindowFrameModel::OnPropertyGridChange::WFBottomCount");
        AddASAPWork(OutputModelManager::WORK_MODELS_CHANGE_REQUIRING_RERENDER, "WindowFrameModel::OnPropertyGridChange::WFBottomCount");
        AddASAPWork(OutputModelManager::WORK_RELOAD_MODEL_FROM_XML, "WindowFrameModel::OnPropertyGridChange::WFBottomCount");
        AddASAPWork(OutputModelManager::WORK_REDRAW_LAYOUTPREVIEW, "WindowFrameModel::OnPropertyGridChange::WFBottomCount");
        AddASAPWork(OutputModelManager::WORK_RELOAD_MODELLIST, "WindowFrameModel::OnPropertyGridChange::WFBottomCount");
        return 0;
    } else if ("WFStartLocation" == event.GetPropertyName()) {
        ModelXml->DeleteAttribute("StartSide");
        ModelXml->DeleteAttribute("Dir");
        ModelXml->AddAttribute("Dir", (event.GetValue().GetLong() == 0 || event.GetValue().GetLong() == 2) ? "L" : "R");
        ModelXml->AddAttribute("StartSide", (event.GetValue().GetLong() == 0 || event.GetValue().GetLong() == 1) ? "T" : "B");
        AddASAPWork(OutputModelManager::WORK_RGBEFFECTS_CHANGE, "WindowFrameModel::OnPropertyGridChange::WFStartLocation");
        AddASAPWork(OutputModelManager::WORK_MODELS_CHANGE_REQUIRING_RERENDER, "WindowFrameModel::OnPropertyGridChange::WFStartLocation");
        AddASAPWork(OutputModelManager::WORK_RELOAD_MODEL_FROM_XML, "WindowFrameModel::OnPropertyGridChange::WFStartLocation");
        AddASAPWork(OutputModelManager::WORK_REDRAW_LAYOUTPREVIEW, "WindowFrameModel::OnPropertyGridChange::WFStartLocation");
        return 0;
    } else if ("WFDirection" == event.GetPropertyName()) {
        ModelXml->DeleteAttribute("Rotation");
        ModelXml->AddAttribute("Rotation", event.GetValue().GetLong() == 0 ? "Clockwise" : "Counter Clockwise");
        AddASAPWork(OutputModelManager::WORK_RGBEFFECTS_CHANGE, "WindowFrameModel::OnPropertyGridChange::WFDirection");
        AddASAPWork(OutputModelManager::WORK_MODELS_CHANGE_REQUIRING_RERENDER, "WindowFrameModel::OnPropertyGridChange::WFDirection");
        AddASAPWork(OutputModelManager::WORK_RELOAD_MODEL_FROM_XML, "WindowFrameModel::OnPropertyGridChange::WFDirection");
        AddASAPWork(OutputModelManager::WORK_REDRAW_LAYOUTPREVIEW, "WindowFrameModel::OnPropertyGridChange::WFDirection");
        return 0;
    }

    return Model::OnPropertyGridChange(grid, event);
}
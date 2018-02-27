#ifndef SEQUENCEVIDEOPREVIEW_H
#define SEQUENCEVIDEOPREVIEW_H

#include "xlGLCanvas.h"

struct AVFrame;
class wxPanel;

class SequenceVideoPreview : public xlGLCanvas
{
public:
   SequenceVideoPreview(wxPanel *parent);

   virtual ~SequenceVideoPreview();

   void Render( AVFrame * frame );
   void Clear();

protected:
   void InitializeGLCanvas() override;

   void reinitTexture( int width, int height );

   void deleteTexture();

private:
   void paint( wxPaintEvent& evt );

   unsigned _TexId;
   int      _TexWidth;
   int      _TexHeight;

   DECLARE_EVENT_TABLE()
};

#endif
#include "IPlugEffect.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

#include "RuntimeObjectSystem/ObjectInterfacePerModule.h"
#include "RuntimeObjectSystem/IObject.h"
#include "IPlugRCCPP_InterfaceIds.h"

RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../IPlug/IPlugParameter", ".cpp" );
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../IPlug/IPlugAPIBase", ".cpp" );
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../IPlug/IPlugPluginBase", ".cpp" );
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../IPlug/IPlugProcessor", ".cpp" );
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../IPlug/IPlugTimer", ".cpp" );
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../IPlug/IPlugPaths", ".cpp" );
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../IPlug/APP/IPlugAPP", ".cpp" );
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../Dependencies/IPlug/RTAudio/RtAudio", ".cpp" );
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../Dependencies/IPlug/RTMidi/RtMidi", ".cpp" );
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../IGraphics/IGraphicsEditorDelegate", ".cpp" );
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../IGraphics/IGraphics", ".cpp" );
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../IGraphics/IControl", ".cpp" );
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../IGraphics/Controls/IControls", ".cpp" );
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../IGraphics/Controls/IPopupMenuControl", ".cpp" );
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../IGraphics/Controls/ITextEntryControl", ".cpp" );
#ifdef OS_WIN
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE( "../../IGraphics/Platforms/IGraphicsWin", ".cpp" );
#endif

#include "RuntimeObjectSystem/RuntimeLinkLibrary.h"
#ifdef _WIN32
    RUNTIME_COMPILER_LINKLIBRARY( "Gdi32.lib");
    RUNTIME_COMPILER_LINKLIBRARY( "opengl32.lib");
    RUNTIME_COMPILER_LINKLIBRARY( "User32.lib");
    RUNTIME_COMPILER_LINKLIBRARY( "Shell32.lib");
    RUNTIME_COMPILER_LINKLIBRARY( "Comdlg32.lib");
    RUNTIME_COMPILER_LINKLIBRARY( "Shlwapi.lib" );
    RUNTIME_COMPILER_LINKLIBRARY( "Ole32.lib" );
    RUNTIME_COMPILER_LINKLIBRARY( "Comctl32.lib" );
    RUNTIME_COMPILER_LINKLIBRARY( "Wininet.lib" );
#elif __APPLE__
    RUNTIME_COMPILER_LINKLIBRARY( "-framework OpenGL");
#elif __linux
    RUNTIME_COMPILER_LINKLIBRARY( "-lOpenGL32");
#endif

class IPlugEffectRCCpp : public TInterface<IID_IPLUGEFFECT, IObject>
{
};
REGISTERCLASS(IPlugEffectRCCpp);


IPlugEffect::IPlugEffect(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPrograms))
{
  GetParam(kGain)->InitDouble("Gain", 0., 0., 100.0, 0.01, "%");

#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new ITextControl(b.GetMidVPadded(50), "Hello iPlug 2!", IText(50)));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100), kGain));
  };
#endif
}

#if IPLUG_DSP
void IPlugEffect::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kGain)->Value() / 100.;
  const int nChans = NOutChansConnected();
  
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = inputs[c][s] * gain;
    }
  }
}
#endif



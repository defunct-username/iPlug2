#include "IPlugEffect.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include <variant>
#include <unordered_map>
#include <optional>
#include <string>

class PropControl : public IControl
{
public:
  using PropVar = std::variant<bool, int, float, const char*, IColor, IRECT>;
  using PropPair = std::pair<const std::string, PropVar>;
  using PropMap = std::unordered_map<std::string, PropVar>;
  
  PropControl(const IRECT& bounds, PropMap properties)
  : IControl(bounds)
  , mProperties(properties)
  {
  }
  
  void SetProperty(const std::string& name, PropVar prop)
  {
    mProperties.insert_or_assign(name, prop);
  }
  
  template<typename T>
  std::optional<T> GetProperty(const std::string& name) const
  {
    auto result = mProperties.find(name);
    // can replace std::get_if with std::get on macOS > 10.14 https://stackoverflow.com/questions/52521388/stdvariantget-does-not-compile-with-apple-llvm-10-0/53887048#53887048
    return result == mProperties.end() ? std::nullopt : std::optional<T>(*std::get_if<T>(&result->second));
  }
  
  void Draw(IGraphics& g) override
  {
    g.FillRect(*GetProperty<IColor>("bgcolor"), mRECT);
    g.FillRect(*GetProperty<IColor>("fgcolor"), mRECT.GetCentredInside(10.f));
  }
  
private:
  PropMap mProperties;
};

IPlugEffect::IPlugEffect(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPrograms))
{
  GetParam(kGain)->InitDouble("Gain", 0., 0., 100.0, 0.01, "%");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new PropControl(b.GetCentredInside(100), {{"bgcolor", COLOR_GREEN}, {"fgcolor", COLOR_BLUE}}));
//    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100), kGain));
//    pGraphics->AttachControl(new IPanelControl(b, COLOR_RED));

//    pGraphics->AttachControl(new IVColorSwatchControl(b.GetCentredInside(200)));
    
//    pGraphics->AttachControl(new IVSliderControl(b.GetCentredInside(30,100), [](IControl* pCaller){
//      dynamic_cast<IVSliderControl*>(pCaller)->SetColor(kX1, IColor::LinearInterpolateBetween(COLOR_RED, COLOR_ORANGE, pCaller->GetValue())); }
//      , "", DEFAULT_STYLE, false, EDirection::Vertical, DEFAULT_GEARING, 0.f, 15.f));
//
//    pGraphics->AttachControl(new IVSliderControl(b.GetCentredInside(30,100).GetHShifted(30), [](IControl* pCaller){
//      dynamic_cast<IVSliderControl*>(pCaller)->SetColor(kX1, IColor::LinearInterpolateBetween(COLOR_RED, COLOR_WHITE, pCaller->GetValue())); },
//      "", DEFAULT_STYLE, false, EDirection::Vertical, DEFAULT_GEARING, 0.f, 15.f));
//    pGraphics->AttachControl(new ILambdaControl(b.GetCentredInside(100),
//      [](ILambdaControl* pCaller, IGraphics& g, IRECT& rect) {
//
//        g.FillRect(COLOR_RED, rect.GetScaledAboutCentre(2.f));
//
//    }, DEFAULT_ANIMATION_DURATION, false /*loop*/, false /*start immediately*/));
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

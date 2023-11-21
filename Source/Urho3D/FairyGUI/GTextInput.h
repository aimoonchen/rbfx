#ifndef __GTEXTINPUT_H__
#define __GTEXTINPUT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GTextField.h"
//#include "display/FUIInput.h"

NS_FGUI_BEGIN

class GTextInput : public GTextField, public cocos2d::TextFieldDelegate//, cocos2d::ui::EditBoxDelegate
{
public:
    GTextInput();
    virtual ~GTextInput();

    CREATE_FUNC(GTextInput);

    virtual bool isSingleLine() const override;
    virtual void setSingleLine(bool value) override;

    virtual TextFormat* getTextFormat() const override { return _textFormat.get(); /*_input->getTextFormat();*/ }
    virtual void applyTextFormat() override;

    void setPrompt(const std::string& value);
    void setPassword(bool value);
    void setKeyboardType(int value);
    void setMaxLength(int value);
    void setRestrict(const std::string& value);

    static void onLastFocus();
    static GTextInput* focus_input_;

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void setup_beforeAdd(ByteBuffer* buffer, int beginPos) override;

    virtual void setTextFieldText() override;

//     virtual void editBoxReturn(cocos2d::ui::EditBox* editBox) override;
//     virtual void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text) override;

    bool onTextFieldInsertText(cocos2d::TextFieldTTF* sender, const char* text, size_t nLen) override;
    bool onTextFieldDeleteBackward(cocos2d::TextFieldTTF* sender, const char* delText, size_t nLen) override;

private:
    //FUIInput* _input;
    std::unique_ptr<TextFormat> _textFormat;
    cocos2d::TextFieldTTF* _input;
    cocos2d::Vec2 _beginPos;
};

NS_FGUI_END

#endif

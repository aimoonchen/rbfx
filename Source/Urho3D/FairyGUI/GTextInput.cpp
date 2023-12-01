#include "GTextInput.h"
#include "UIPackage.h"
#include "GRoot.h"
//#include "ui/UIEditBox/UIEditBox.h"
#include "utils/ByteBuffer.h"
#include "utils/UBBParser.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;

void (*updateIMEPosition)(int, int) = nullptr;

GTextInput::GTextInput()
{
    _textFormat = std::make_unique<TextFormat>();
}

GTextInput::~GTextInput()
{
}

GTextInput* GTextInput::focus_input_ = nullptr;

void GTextInput::handleInit()
{
    //_input = FUIInput::create();
    _input = TextFieldTTF::textFieldWithPlaceHolder("<click for input>", UIConfig::getRealFontName(_textFormat->face).c_str(), _textFormat->fontSize);
    _input->retain();
    _input->setDelegate(this);

    _displayObject = _input;

//     this->addEventListener(UIEventType::TouchEnd, [this](EventContext*) {
//         _input->openKeyboard();
//     });
    this->addEventListener(UIEventType::TouchBegin, [this](EventContext* event) {
        _input->attachWithIME();
        auto pos = localToGlobal(Vec2::ZERO);
        updateIMEPosition((int)pos.x, (int)pos.y);
        GTextInput::focus_input_ = this;
    });
}

bool GTextInput::isSingleLine() const
{
    return true;// _input->isSingleLine();
}

void GTextInput::setSingleLine(bool value)
{
    //_input->setSingleLine(value);
}

void GTextInput::applyTextFormat()
{
    //_input->applyTextFormat();
    TTFConfig ttfConfig(UIConfig::getRealFontName(_textFormat->face).c_str(), _textFormat->fontSize, GlyphCollection::DYNAMIC);
    _input->setTTFConfig(ttfConfig);
    _input->setTextColor(Color4B{ _textFormat->color });
}

void GTextInput::setPrompt(const std::string & value)
{
    if (value.empty())
        _input->setPlaceHolder(value.c_str());
    else
    {
        UBBParser* parser = UBBParser::getInstance();
        _input->setPlaceHolder(parser->parse(value.c_str(), true).c_str());
        if (!parser->lastColor.empty())
            _input->setColorSpaceHolder(ToolSet::hexToColor(parser->lastColor.c_str()));
//             _input->setPlaceholderFontColor(ToolSet::hexToColor(parser->lastColor.c_str()));
//         if (!parser->lastFontSize.empty())
//             _input->setPlaceholderFontSize(Value(parser->lastFontSize).asInt());
    }
}

void GTextInput::setPassword(bool value)
{
    //_input->setPassword(value);
    _input->setSecureTextEntry(value);
}

void GTextInput::setKeyboardType(int value)
{
    //_input->setKeyboardType(value);
}

void GTextInput::setMaxLength(int value)
{
    //_input->setMaxLength(value);
}

void GTextInput::setRestrict(const std::string & value)
{
}

void GTextInput::handleSizeChanged()
{
    _input->setContentSize(_size);
}

void GTextInput::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GTextField::setup_beforeAdd(buffer, beginPos);

    buffer->seek(beginPos, 4);

    const std::string* str;
    if ((str = buffer->readSP()))
        setPrompt(*str);

    if ((str = buffer->readSP()))
        setRestrict(*str);

    int iv = buffer->readInt();
    if (iv != 0)
        setMaxLength(iv);
    iv = buffer->readInt();
    if (iv != 0)
        setKeyboardType(iv);
    if (buffer->readBool())
        setPassword(true);
}

void GTextInput::setTextFieldText()
{
//     if (_templateVars != nullptr)
//         _input->setText(parseTemplate(_text.c_str()));
//     else
//         _input->setText(_text);
    if (_templateVars != nullptr)
        _input->setString(parseTemplate(_text.c_str()));
    else
        _input->setString(_text);
}

// void GTextInput::editBoxReturn(cocos2d::ui::EditBox * editBox)
// {
//     //found that this will trigger even when focus is lost
//     //if (isSingleLine())
//     // dispatchEvent(UIEventType::Submit);
// }
// 
// void GTextInput::editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text)
// {
//     _text.clear();
//     _text.append(_input->getText());
// }

bool GTextInput::onTextFieldInsertText(TextFieldTTF* sender, const char* text, size_t nLen)
{
    _text.clear();
    _text.append(_input->getString());
    return false;
}

bool GTextInput::onTextFieldDeleteBackward(TextFieldTTF* sender, const char* delText, size_t nLen)
{
    return false;
}

void GTextInput::onLastFocus()
{
    if (focus_input_)
    {
        focus_input_->_input->detachWithIME();
        focus_input_ = nullptr;
    }
}
NS_FGUI_END

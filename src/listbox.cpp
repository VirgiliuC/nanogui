/*
    src/listbox.cpp -- A classical list box, single or multi selection
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    ListBox is contributed by Virgiliu Crãciun as part of NanoAUI

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/


#include <nanogui/listbox.h>
#include <nanogui/theme.h>
#include <nanogui/serializer/core.h>
#include <cassert>
#include <algorithm>

NAMESPACE_BEGIN(nanogui)
ListBox::ListBoxItem::ListBoxItem(Widget* parent, std::string caption,
                 void* client_data)
                    : ListBase::ListItemBase(parent, caption,client_data),
                    mCaption(caption), mSelected(false),
                    mIcon(0),mIconPosition(Button::IconPosition::Left){};


Vector2i ListBox::ListBoxItem::preferredSize(NVGcontext *ctx) const {
    int fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
    nvgFontSize(ctx, fontSize);
    nvgFontFaceId(ctx, mTheme->mFontNormal);
    float tw = nvgTextBounds(ctx, 0,0, mCaption.c_str(), nullptr, nullptr);
    float iw = 0.0f, ih = fontSize;

    if (mIcon) {
        if (nvgIsFontIcon(mIcon)) {
            ih *= 1.5f;
            nvgFontFaceId(ctx, mTheme->mFontIcons);
            nvgFontSize(ctx, ih);
            iw = nvgTextBounds(ctx, 0, 0, utf8(mIcon).data(), nullptr, nullptr)
                + mSize.y() * 0.15f;
        } else {
            int w, h;
            ih *= 0.9f;
            nvgImageSize(ctx, mIcon, &w, &h);
            iw = w * ih / h;
        }
    }
    return Vector2i((int)(tw + iw) + 20, fontSize + 10);
}

void ListBox::ListBoxItem::draw(NVGcontext *ctx){
    //basic draw: a background and a border
     ListItemBase::draw(ctx);

    int fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
    nvgFontSize(ctx, fontSize);
    nvgFontFaceId(ctx, mTheme->mFontNormal);
    float tw = nvgTextBounds(ctx, 0,0, mCaption.c_str(), nullptr, nullptr);

    Vector2f center = mPos.cast<float>() + mSize.cast<float>() * 0.5f;
    Vector2f textPos(mPos.x()+5, center.y() - 1);
    NVGcolor textColor =mTheme->mTextColor;///???
//        mTextColor.w() == 0 ? mTheme->mTextColor : mTextColor;
    if (!enabledStatus())
        textColor = mTheme->mDisabledTextColor;
    //draw the icon, if any
    if (mIcon) {
        auto icon = utf8(mIcon);

        float iw, ih = fontSize;
        if (nvgIsFontIcon(mIcon)) {
            ih *= 1.5f;
            nvgFontSize(ctx, ih);
            nvgFontFaceId(ctx, mTheme->mFontIcons);
            iw = nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, nullptr);
        } else {
            int w, h;
            ih *= 0.9f;
            nvgImageSize(ctx, mIcon, &w, &h);
            iw = w * ih / h;
        }
        if (mCaption != "")
            iw += mSize.y() * 0.15f;
        nvgFillColor(ctx, textColor);
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        Vector2f iconPos;
        iconPos.y() = center.y()-1;

        if (mIconPosition == Button::IconPosition::Right) {
            iconPos.x() = mPos.x() + mSize.x() - iw - 8;
        }else if (mIconPosition == Button::IconPosition::Left) {
            iconPos.x() = mPos.x() + 8;
            textPos.x() = iconPos.x()+ iw +8;//text on icon's right
        }if (mCaption != ""){
            iconPos.x() = textPos.x() + tw +8;
        }else{
            iconPos.x() = center.x() - iw*0.5f;;
        }

        if (nvgIsFontIcon(mIcon)) {
            nvgText(ctx, iconPos.x(), iconPos.y()+1, icon.data(), nullptr);
        } else {
            NVGpaint imgPaint = nvgImagePattern(ctx,
                    iconPos.x(), iconPos.y() - ih/2, iw, ih, 0, mIcon, enabledStatus() ? 0.5f : 0.25f);
            nvgFillPaint(ctx, imgPaint);
            nvgFill(ctx);
        }
    }
    //draw the label
    if (mCaption != ""){
        nvgFontSize(ctx, fontSize);
        nvgFontFaceId(ctx, mTheme->mFontNormal);
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgFillColor(ctx, mTheme->mTextColorShadow);
        nvgText(ctx, textPos.x(), textPos.y(), mCaption.c_str(), nullptr);
        nvgFillColor(ctx, textColor);
        nvgText(ctx, textPos.x(), textPos.y() + 1, mCaption.c_str(), nullptr);
    }
}

void ListBox::ListBoxItem::save(Serializer &s) const {
    Widget::save(s);
    s.set("caption", mCaption);
    s.set("icon", mIcon);
    s.set("iconPosition", (int) mIconPosition);
    s.set("selected", mSelected);
//    s.set("backgroundColor", mBackgroundColor);
//    s.set("textColor", mTextColor);
}

bool ListBox::ListBoxItem::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("caption", mCaption)) return false;
    if (!s.get("icon", mIcon)) return false;
    if (!s.get("iconPosition", mIconPosition)) return false;
    if (!s.get("selected", mSelected)) return false;
//    if (!s.get("backgroundColor", mBackgroundColor)) return false;
//    if (!s.get("textColor", mTextColor)) return false;
    return true;
}

////////////////////////////////////////////////////////////
ListBox::ListBox(Widget *parent, unsigned flags):
        ListBase(parent,flags) {setId("ListBox");}

ListBox::ListBox(Widget *parent,
                 const std::vector<std::string> &tags,
                 unsigned flags)
    :ListBase(parent,tags, flags) {setId("ListBox");}


void ListBox::setIconPosition(Button::IconPosition iconPosition) {
    mIconPosition = iconPosition;
    for (size_t i=0; i < count(); i++)
        getItem<ListBoxItem>(i)->setIconPosition(mIconPosition);
}


NAMESPACE_END(nanogui)

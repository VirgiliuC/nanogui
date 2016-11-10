/*
    src/vscrollpanel.cpp -- Adds a vertical scrollbar around a widget
    that is too big to fit into a certain area

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/vscrollpanel.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

const int scroller_width = 12;
const int scroller_end_padding = 4;
const int knob_width = 8;

VScrollPanel::VScrollPanel(Widget *parent)
    : Widget(parent), mChildPreferredHeight(0), mScroll(0.0f)
    {    }


//************ Styling ******************************/
void VScrollPanel::performLayout(NVGcontext *ctx) {
    //don't use layout on  this widget
    mLayout = nullptr;
    Widget::performLayout(ctx);

    if (mChildren.empty())
        return;
    //confine own height to window or parent, to let scrolling to kick in and do its job properly
    if(mFixedSize.y() == 0.0){
        if(window()){//first, confine to window
            int ypos_in_window = absolutePosition().y() - window()->absolutePosition().y();
            mSize.y() =  std::min(mSize.y(), window()->height() - ypos_in_window);
        }
        if(parent()){//second, further confine to parent
            mSize.y() =  std::min(mSize.y(), parent()->height() - mPos.y());
        }
    }
    Widget *child = mChildren[0];
    mChildPreferredHeight = child->preferredSize(ctx).y();
    //translate the child based on the current scroll shift
    child->setPosition(Vector2i(0, -mScroll*(mChildPreferredHeight - mSize.y())));
    child->setSize(Vector2i(mSize.x()-scroller_width, mChildPreferredHeight));
    child->performLayout(ctx);
}

Vector2i VScrollPanel::preferredSize(NVGcontext *ctx) const {
    if (mChildren.empty())
        return Vector2i::Zero();
    return mChildren[0]->preferredSize(ctx) + Vector2i(scroller_width, 0);
}

bool VScrollPanel::mouseDragEvent(const Vector2i &p, const Vector2i &rel,
                            int button, int modifiers) {

    if (!mChildren.empty() && mChildPreferredHeight > mSize.y()) {

        float knob_height = height() *
            std::min(1.0f, height() / (float)mChildPreferredHeight);

        mScroll = std::max((float) 0.0f, std::min((float) 1.0f,
                     mScroll + rel.y() / (float)(mSize.y() - 2*scroller_end_padding - knob_height)));
        return true;
    } else {
        return Widget::mouseDragEvent(p, rel, button, modifiers);
    }
}

bool VScrollPanel::scrollEvent(const Vector2i &p , const Vector2f &rel) {
    if (!mChildren.empty() && mChildPreferredHeight > mSize.y()) {
        float knob_height = height() *
            std::min(1.0f, height() / (float)mChildPreferredHeight);

        mScroll = std::max((float) 0.0f, std::min((float) 1.0f,
                     mScroll + rel.y() / (float)(mSize.y() - 2*scroller_end_padding - knob_height)));
        return true;
    } else {
        return Widget::scrollEvent(p, rel);
    }
}

void VScrollPanel::draw(NVGcontext *ctx) {
    if (mChildren.empty())
        return;
    Widget *child = mChildren[0];
    mChildPreferredHeight = child->preferredSize(ctx).y();
    float knob_height = height() *
        std::min(1.0f, height() / (float) mChildPreferredHeight);

    nvgSave(ctx);
    nvgTranslate(ctx, mPos.x(), mPos.y());
    nvgIntersectScissor(ctx, 0, 0, mSize.x(), mSize.y());
    performLayout(ctx);
    if (child->visible()){
        child->draw(ctx);
    }
    nvgRestore(ctx);
    //don't draw the scroller if there's nothing to scroll
    if(mChildren[0]->height() <= height())
        return;
    //draw the scroller
    NVGpaint paint = nvgBoxGradient(ctx,
        mPos.x() + mSize.x() - scroller_width + 1,
        mPos.y() + scroller_end_padding + 1,
        knob_width,
        mSize.y() - 2*scroller_end_padding,
        3, 4, Color(0, 32), Color(0, 92));
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx,
                   mPos.x() + mSize.x() - scroller_width,
                   mPos.y() + scroller_end_padding,
                   knob_width,
                   mSize.y() - 2*scroller_end_padding, 3);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);

    //draw the scroller knob
    paint = nvgBoxGradient(ctx,
        mPos.x() + mSize.x() - scroller_width - 1,
        mPos.y() + scroller_end_padding + (mSize.y() - 2*scroller_end_padding - knob_height) * mScroll - 1,
        knob_width, knob_height,
        3, 4, Color(220, 100), Color(128, 100));
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx,
                   mPos.x() + mSize.x() - scroller_width + 1,
                   mPos.y() + scroller_end_padding + 1 + (mSize.y() - 2*scroller_end_padding - knob_height) * mScroll,
                   knob_width - 2,
                   knob_height - 2, 2);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);
}
//************ Serialize ******************************/

void VScrollPanel::save(Serializer &s) const {
    Widget::save(s);
    s.set("childPreferredHeight", mChildPreferredHeight);
    s.set("scroll", mScroll);
}

bool VScrollPanel::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("childPreferredHeight", mChildPreferredHeight)) return false;
    if (!s.get("scroll", mScroll)) return false;
    return true;
}

////////////////////////////////////////////////////////////////////////

VScrollContainer::VScrollContainer(Widget *parent)
    : Widget(nullptr)
{
        mVScrollPanel = new VScrollPanel(parent);
        mVScrollPanel->addChild(this);
        if (parent) {
            mTheme = parent->theme();
        }
}
NAMESPACE_END(nanogui)

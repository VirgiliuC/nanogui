/*
    nanogui/vscroll.h -- Adds a vertical scrollbar around a widget
    that is too big to fit into a certain area

    VScrollContainer has been contributed by Virgiliu Crãciun

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class VScrollPanel vscrollpanel.h nanogui/vscrollpanel.h
 *
 * \brief Adds a vertical scrollbar around a widget that is too big to fit into
 *        a certain area.
 */
class NANOGUI_EXPORT VScrollPanel : public Widget {
public:
    VScrollPanel(Widget *parent);

    virtual void performLayout(NVGcontext *ctx) override;
    virtual Vector2i preferredSize(NVGcontext *ctx) const override;
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    virtual bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;
    virtual void draw(NVGcontext *ctx) override;
    virtual void save(Serializer &s) const override;
    virtual bool load(Serializer &s) override;

protected:
    int mChildPreferredHeight;
    float mScroll;
};

//***********************************************************
//
///**
// * \class VScrollContainer vscroll.h nanogui/vscroll.h
// *
// * \brief A vertical scroll control usind VScrollPanel as the scrolling mechanism.
// */
//class NANOGUI_EXPORT VScrollContainer : public Widget {
//public:
//    VScrollContainer(Widget *parent);
//
//        /// set the size of the widget
//    void setSize(const Vector2i &size)
//        { mVScrollPanel->setSize(size);}
//
//    /// Set the width of the widget
//    void setWidth(int width)
//        { mVScrollPanel->setWidth(width);}
//
//    /// Set the height of the widget
//    void setHeight(int height)
//        {mVScrollPanel->setHeight(height);}
//    /**
//     * \brief Set the fixed size of this widget
//     *
//     * If nonzero, components of the fixed size attribute override any values
//     * computed by a layout generator associated with this widget, but parent's fixed size takes priority.
//     * Note that just setting the fixed size alone is not enough to actually change its
//     * size; this is done with a call to \ref setSize or a call to \ref performLayout()
//     * in the parent widget.
//     */
//    void setFixedSize(const Vector2i &size)
//        { mVScrollPanel->setFixedSize(size);}
//    /// Set the fixed width (see \ref setFixedSize())
//    void setFixedWidth(int width)
//        { mVScrollPanel->setFixedWidth(width);}
//    /// Set the fixed height (see \ref setFixedSize())
//    void setFixedHeight(int height)
//        { mVScrollPanel->setFixedHeight(height);}
//
//    const std::vector<Widget *> &containedChildren() const
//                { return mChildren; }
//
//
//    /// Invoke the associated layout generator to properly place child widgets, if any
//    virtual void performLayout(NVGcontext *ctx);
//private:
//    ref<VScrollPanel> mVScrollPanel;
//};

NAMESPACE_END(nanogui)

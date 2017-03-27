/*
    nanogui/listbox.h -- a list box using a scroll panel, single or multiple selection

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    ListBox was contributed by Virgiliu Crãciun

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/listbase.h>
#include <nanogui/button.h>

NAMESPACE_BEGIN(nanogui)


/**
 * \class ListBox listbox.h nanogui/listbox.h
 *
 * \brief Simple list box widget with single or multiple selections, sorted or not.
 */

class NANOGUI_EXPORT ListBox : public ListBase {
public:

    /// Create an empty list box
    ListBox(Widget *parent, unsigned flags);

    /**
     * \brief Create a new list box with the given tags
     */
    ListBox(Widget *parent, const std::vector<std::string> &tags, unsigned flags);
    ~ListBox(){};

    int icon(size_t idx) const
        { return idx < count() ? getItem<ListBoxItem>(idx)->icon() : -1;}
    void setIcon(size_t idx, int icon)
        { if(idx < count())getItem<ListBoxItem>(idx)->setIcon(icon); }

    Button::IconPosition iconPosition() const { return mIconPosition; }
    void setIconPosition(Button::IconPosition iconPosition);

protected:

    /**
     * \class ListItemBase listbase.h nanogui/listbase.h
     *
     * \brief List item derived from widget, a pure virtual base for all list items.
     */
    class NANOGUI_EXPORT ListBoxItem : public ListBase::ListItemBase{

    public:
        ListBoxItem(Widget* parent, std::string caption,void* client_data=nullptr);

        void    setTag(std::string tag) {mCaption = tag;}
        std::string tag() const {return mCaption;}

        virtual bool    selected() const {return mSelected;};
        virtual bool    hasString(const std::string& str) const
                            {return mCaption.find(str) != std::string::npos;};

        int     icon() const { return mIcon; }
        void    setIcon(int icon) { mIcon = icon; }
        Button::IconPosition iconPosition() const { return mIconPosition; }
        void    setIconPosition(Button::IconPosition iconPosition) { mIconPosition = iconPosition; }

        virtual void save(Serializer &s) const override;
        virtual bool load(Serializer &s) override;

    protected:
        void    select() {mSelected = enabled() ? true : false;}
        void    deselect() {mSelected = false;}

        virtual Vector2i preferredSize(NVGcontext *ctx) const;
        virtual void     draw(NVGcontext *ctx) override;
    /** Listbox item does not support any children.*/
        virtual void addChild(int index, Widget *widget){};

    private:
        std::string mCaption;
        bool mSelected;
        int  mIcon;
        Button::IconPosition  mIconPosition;
    };

protected:

    virtual ListItemBase* createItem(std::string caption,
                                void* client_data=nullptr){
                                    return new ListBoxItem(nullptr,caption,client_data);
                                };

    Button::IconPosition  mIconPosition;

protected:

};

/****************************************/


NAMESPACE_END(nanogui)

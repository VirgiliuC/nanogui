/*
    nanogui/listbox.h -- a list box using a scroll panel, single or multiple selection

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    ListBase was contributed by Virgiliu Crãciun

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/vscrollpanel.h>
#include <nanogui/glutil.h>

NAMESPACE_BEGIN(nanogui)


typedef std::function<void(int,bool)> SelectCallback;
/**
 * \class ListBase listbox.h nanogui/listbox.h
 *
 * \brief Simple list box widget with single or multiple selections, sorted or not.
 */

class NANOGUI_EXPORT ListBase : public Widget {
public:

    friend class ListSorter;

    enum Flags {None=0,Multiple=0x0001, Sorted=0x0002};

    /// Create an empty list box
    ListBase(Widget *parent, unsigned flags);

    /**
     * \brief Create a new list box with the given tags
     */
    ListBase(Widget *parent, const std::vector<std::string> &tags, unsigned flags);

    size_t count() const {return childCount();};

    int  append(std::string tag, void* client_data=nullptr);
    void append(const std::vector<std::string> &tags);
    void append(const std::vector<std::string> &tags,
                const std::vector<void*> &clientData);
    bool insert(unsigned pos, std::string tag, void* client_data=nullptr) ;
    bool insert(unsigned pos, const std::vector<std::string> &tags);
    bool insert(unsigned pos, const std::vector<std::string> &tags,
                const std::vector<void*> &clientData);
/** Swaps the order of two list entries in unsorted lists (no effect on sorted ones).*/
    int  swap(unsigned, unsigned);
/** Locate the positional index of an entry based on its tag.*/
    int  find(std::string strtag, unsigned pos=0) ;
    void remove(std::string tag) {removeChild(find(tag));};
    void remove(unsigned i){removeChild(i);};
    void clear() {removeAllChildren();}
/** Set all the items (removing any existing ones) based on tags supplied.*/
    void setItems(const std::vector<std::string> &tags);
/** Set all the items (removing any existing ones) based on tags supplied, adding the client data as well.*/
    void setItems(const std::vector<std::string> &tags,
                const std::vector<void*> &clientData);
    std::string tag(size_t i) const {return getItem<ListItemBase>(i)->tag();}
/** Convenience function for setting the client data pointer at a given list index.*/
    const void* clientData(unsigned i) const
                    { auto pi = getItem<ListItemBase>(i); return pi ? pi->clientData() : nullptr; }
    void setClientData(unsigned i, void* client_data)
                    { auto pi = getItem<ListItemBase>(i);  if(pi) pi->setClientData(client_data); }
/** Display those elements whose tag contains the substr.
    If 'at_start' is true, only tags starting with substring are considered*/
    void filterByTag(const std::string& substr, bool at_start);
/** Display those elements who return true on 'hasString(substr)'.*/
    void filterGeneric(const std::string& substr);
/** Show/hide a list item.*/
    void itemVisible(unsigned i, bool visible){auto pi = getItem<ListItemBase>(i);if(pi) pi->setVisible(visible);};
/** Enable/disable a list item.*/
    void itemEnable(unsigned i, bool enable){auto pi = getItem<ListItemBase>(i);  if(pi) pi->setEnabled(enable);};
/** Remove completely any filtration by showing all registered entries.*/
    void showAll();

    SelectCallback callback() const { return mCallback; }
    void setCallback(const SelectCallback &callback) { mCallback = callback; }

/** Select the given index, if in range*/
    bool setSelection(unsigned idx);
/** Get the selected indices*/
    void getSelection(std::vector<int>& selection) const;
/** Returns the last selected index for single mode, and last selected item for multiple*/
    int  lastSelected() const { return mLastSelected;}
/** Check if the item at the given index is selected.*/
    bool isSelected(int i) const {auto pi = getItem<ListItemBase>(i); return pi? pi->selected():false;}
/** Deselect the given item; applies only for multiple selections.*/
    void    deselect(unsigned);
/** Remove completely all selections.*/
    void    deselectAll();

/** Manual (on demand) sort of the list*/
    virtual void sort();

    /// Handle a keyboard event (default implementation: move selected with arrow keys)
    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);

    virtual void save(Serializer &s) const override;
    virtual bool load(Serializer &s) override;

    static bool sortByTag(const Widget* a, const Widget* b);

protected:
    /**
 * \class ListItemBase listbase.h nanogui/listbase.h
 *
 * \brief List item derived from widget, a pure virtual base for all list items.
 */
    class ListItemBase : public Widget{

        public:
        ListItemBase(Widget* parent, std::string caption,
                 void* client_data=nullptr);

        virtual void    setTag(std::string)=0;
        virtual std::string tag() const=0;

        void    setClientData(void* pdata) {mClientData=pdata;};
        void*   clientData(){return mClientData;};
        const
        void*   clientData() const {return mClientData;};

        virtual bool    selected() const=0;
        virtual bool    hasString(const std::string&) const =0;
        virtual void    select()=0;
        virtual void    deselect()=0;
        virtual void    toggleSelect(){ selected()?deselect():select();};

        protected:
    /** Draws a bordered background as canvas for derived items, based on the current theme.*/
        virtual void    draw(NVGcontext *ctx) override;

        private:
        void   *mClientData;
    };

protected:
    virtual ListItemBase* createItem(std::string /*caption*/,
                                void* client_data=nullptr){return nullptr;};

    virtual void addChild(int index, Widget *widget);
    /** Get the list item at the given index (constant version).*/
    template<class T>
    T* getItem(unsigned i) {return dynamic_cast<T*>(childAt(i));}
/** Get the list item at the given index (constant version).*/
    template<class T>
    const T* getItem(unsigned i) const {return dynamic_cast<const T*>(childAt(i));}

    void selectRange(int);

private:

    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers);

protected:
    unsigned mFlags;
    int mLastSelected;
    int mSelectAnchor;
    SelectCallback mCallback;
};


NAMESPACE_END(nanogui)

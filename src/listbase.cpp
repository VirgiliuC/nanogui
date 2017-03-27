/*
    src/listbase.cpp -- implements list controls functionality
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    ListBase is contributed by Virgiliu Crãciun as part of NanoAUI

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
#include <nanogui/listbase.h>
#include <nanogui/layout.h>
#include <nanogui/theme.h>
#include <nanogui/serializer/core.h>
#include <cassert>
#include <algorithm>
#include <iostream>

NAMESPACE_BEGIN(nanogui)

ListBase::ListItemBase::ListItemBase(Widget* parent,
                   std::string /*caption*/,
                   void* client_data)
: Widget(parent), mClientData(client_data){

}


void ListBase::ListItemBase::draw(NVGcontext *ctx){
//     Widget::draw(ctx);

        NVGpaint ibg;

    if (mMouseFocus) {
        nvgBeginPath(ctx);
        nvgRoundedRect(ctx, mPos.x() + 1, mPos.y() + 1.0f, mSize.x() - 2,
                       mSize.y() - 2, mTheme->mButtonCornerRadius - 1);
        ibg = nvgLinearGradient(ctx, mPos.x(), mPos.y(), mPos.x(),
                                    mPos.y() + mSize.y(),
                                    mTheme->mChoiceGradientTopHovered,
                                    mTheme->mChoiceGradientBotHovered);
        nvgFillPaint(ctx, ibg);
        nvgFill(ctx);
        //draw the dark border
        nvgBeginPath(ctx);
        nvgRoundedRect(ctx, mPos.x() + 0.5f, mPos.y() + 0.5f, mSize.x() - 1,
                       mSize.y() - 2, mTheme->mButtonCornerRadius);
        nvgStrokeColor(ctx, mTheme->mBorderDark);
        nvgStroke(ctx);
    }

    if (selected()) {
        nvgBeginPath(ctx);
        nvgRoundedRect(ctx, mPos.x() + 1, mPos.y() + 1.0f, mSize.x() - 2,
                       mSize.y() - 2, mTheme->mButtonCornerRadius - 1);
        ibg = nvgLinearGradient(ctx, mPos.x(), mPos.y(), mPos.x(),
                                    mPos.y() + mSize.y(),
                                    mTheme->mChoiceGradientTopSelected,
                                    mTheme->mChoiceGradientBotSelected);
        nvgFillPaint(ctx, ibg);
        nvgFill(ctx);
        //draw the light border
        nvgBeginPath(ctx);
        nvgStrokeWidth(ctx, 1.0f);
        nvgRoundedRect(ctx, mPos.x() + 0.5f, mPos.y() + (selected() ? 0.5f : 1.5f), mSize.x() - 1,
                       mSize.y() - 1 - (selected() ? 0.0f : 1.0f), mTheme->mButtonCornerRadius);
        nvgStrokeColor(ctx, mTheme->mBorderLight);
        nvgStroke(ctx);
    }
}

////////////////////////////////////////////////////////////////////////////////////
bool ListBase::sortByTag(const Widget* a, const Widget* b){
    const ListBase::ListItemBase* i1 = dynamic_cast<const ListBase::ListItemBase*>(a);
    const ListBase::ListItemBase* i2 = dynamic_cast<const ListBase::ListItemBase*>(b);
    return (i1 and i2 ? i1->tag() < i2->tag() : false);
}
//
//bool operator==(const Widget* a, const std::string &s){
//    const ListBase::ListItemBase* i1 = dynamic_cast<const ListBase::ListItemBase*>(a);
//    return (i1 ? i1->tag() == s : false);
//}

/////////////////////////////////////////////////////////////////////////////////


ListBase::ListBase(Widget *parent, unsigned flags)
: Widget(parent), mFlags(flags), mCallback(nullptr){

    auto table = new GridLayout(/*dir*/Orientation::Horizontal, /*dir size*/1,
                                      Alignment::Fill, /*margin*/2,/*spacing*/1);
    setLayout(table);
//    mSorter=&pLI_less_than;

}

ListBase::ListBase(Widget *parent,
                 const std::vector<std::string> &tags, unsigned flags)
: Widget(parent), mFlags(flags), mCallback(nullptr) {

    auto grid = new GridLayout(/*dir*/Orientation::Horizontal, /*dir size*/1,
                                      Alignment::Fill, /*margin*/1,/*spacing*/0);
    setLayout(grid);
    setItems(tags);
//    mSorter=ListSorter();&pLI_less_than;
}


int ListBase::append(std::string tag, void* client_data){
    //use a null parent first, otherwise the dynamic_cast in 'addChild' will fail
    auto pi = createItem( tag,client_data);
    //then add the child to the parent manually
    addChild(mChildren.size(), pi);
    return std::distance(mChildren.begin(),
                         std::find(mChildren.begin(),mChildren.end(), (Widget*)pi));
}

void ListBase::append(const std::vector<std::string> &tags){
    for(auto s : tags){
        addChild(childCount(), createItem(s,nullptr));
    }
}

void ListBase::append(const std::vector<std::string> &tags,
                const std::vector<void*> &clientData){

    assert(tags.size() == clientData.size());
    int i=0;
    for(auto s : tags){
        addChild(childCount(), createItem(s,clientData[i++]));
    }
}

bool ListBase::insert(unsigned pos, std::string label,
                void* client_data){

    if(pos and pos > (size_t)childCount())
        return false;
    addChild(pos, createItem(label,client_data));
    return true;
}

bool ListBase::insert(unsigned pos, const std::vector<std::string> &tags){

    if(pos and pos > count())
        return false;
    for(auto s : tags){
        addChild(pos++, createItem(s,nullptr));
    }
    return true;
}

bool ListBase::insert(unsigned pos, const std::vector<std::string> &tags,
                const std::vector<void*> &clientData){

    if(pos and pos > count())
        return false;
    assert(tags.size() == clientData.size());
    int i=0;
    for(auto s : tags){
        addChild(pos++, createItem(s,clientData[i++]));
    }
    return true;
}

int ListBase::swap(unsigned a, unsigned b){
    if((mFlags & Sorted) or a >= count() or b>= count())
        return -1;//nonsense for sorted lists
    std::swap(mChildren[a], mChildren[b]);
    return 0;
}

int ListBase::find(std::string strtag, unsigned pos){
    for(size_t i = pos; i < count(); ++i)
        if(tag(i) == strtag)
            return i;
    return -1;
}

void ListBase::setItems(const std::vector<std::string> &tags){
    removeAllChildren();
    for(auto s : tags){
         addChild(childCount(), createItem(s,nullptr));
    }
}

void  ListBase::setItems(const std::vector<std::string> &tags,
         const std::vector<void*> &clientData){
    assert(tags.size() == clientData.size());
    int i=0;
    removeAllChildren();
    for(auto s : tags){
         addChild(childCount(), createItem(s,clientData[i++]));
    }
}

void ListBase::filterByTag(const std::string& substr, bool at_start){
    for(auto b : children()){
         size_t pos =//where the string segment is located
            dynamic_cast<ListBase::ListItemBase*>(b)->tag().find(substr);
        b->setVisible(at_start ? pos==0 : pos != std::string::npos);
    }
}
void ListBase::filterGeneric(const std::string& substr){
    for(auto b : children()){
         bool visible =//the string must be placed at the beginning
            dynamic_cast<ListBase::ListItemBase*>(b)->hasString(substr);
        b->setVisible(visible);
    }
}

void ListBase::showAll(){
    for(auto b : children()){
         b->setVisible(true);
    }
}

bool ListBase::setSelection(unsigned idx) {

    if(idx and idx >=count())
        return false;
    if(not(mFlags & Multiple) and mLastSelected >=0){
        //deselect the previous one in 'single'mode
        getItem<ListBase::ListItemBase>(mLastSelected)->deselect();
    }
    auto pi = getItem<ListBase::ListItemBase>(idx);
    if(pi){
        pi->select();
        mSelectAnchor = mLastSelected = idx;
    }
    return true;
}

void ListBase::getSelection(std::vector<int>& selection) const{
    selection.clear();
    int i=0;
    for(auto b : children()){
         if(dynamic_cast<ListBase::ListItemBase*>(b)->selected())
            selection.push_back(i);
         i++;
    }
}

void ListBase::deselect(unsigned idx) {
    if(not(mFlags & Multiple))
        return;//don't deselect single selection
    auto pi = getItem<ListBase::ListItemBase>(idx);
    if(pi){
        pi->deselect();
        mLastSelected=idx;
    }
}


void ListBase::deselectAll(){
    for(auto b : children()){
        auto pli = dynamic_cast<ListBase::ListItemBase*>(b);
        bool was_selected = pli->selected();
        pli->deselect();
        if(mCallback and was_selected)//notify state change only
            mCallback(childIndex(pli),false);
    }
}

void ListBase::selectRange(int curr){
    int start = std::min(mSelectAnchor, curr);
    int end   = std::max(mSelectAnchor, curr);
    for(int i=start; i <=end; i++){
        auto pli = getItem<ListBase::ListItemBase>(i);
        bool was_selected = pli->selected();
        pli->select();
        if(mCallback and not was_selected)//notify state change only
            mCallback(i,true);
    }
    mLastSelected = curr;
}

bool ListBase::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
//    bool res = Widget::mouseButtonEvent(p, button, down, modifiers);
    //so, here is the selection mechanism:
    if (button == GLFW_MOUSE_BUTTON_1 and down and mEnabled) {
        if(not mFocused)
            requestFocus();
        if(not(mFlags & Multiple) or not (GLFW_MOD_CONTROL & modifiers))
            deselectAll();//clean up any prev selection; notify the state change
        ListBase::ListItemBase *pli;
        size_t idx = mChildren.size()-1;
        for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
            pli = dynamic_cast<ListBase::ListItemBase*>(*it);
            //do a hit test
            if (pli->enabled() and //must be enabled
                pli->visible() and //must be also visible
                pli->contains(p - mPos)){//and must get the hit
                //deal with multiple selections if necessary
                if((mFlags & Multiple) and (modifiers & GLFW_MOD_SHIFT)){//range selection; takes priority
                    selectRange(idx);//this will also notify the state change
                    return true;//don't change the selection anchor
                }
                if((mFlags & Multiple) and (modifiers & GLFW_MOD_CONTROL))//toggle selection
                    pli->toggleSelect();
                else//just select the current one
                    pli->select();
                mSelectAnchor = mLastSelected = idx;//change the selection anchor
                if(mCallback)
                    mCallback(idx,pli->selected());
                return true;
            }
            --idx;//next child, back to front
        }
    }
    return false;
}


bool ListBase::keyboardEvent(int key, int /*scancode*/, int action, int modifiers) {
    if((key != GLFW_KEY_UP and key != GLFW_KEY_DOWN) or
        (action != GLFW_PRESS and action != GLFW_REPEAT))
        return false;//no interest
    int new_sel;
    if(key == GLFW_KEY_UP){
        if(mLastSelected == 0) return true;
        new_sel = mLastSelected < 0 ? count()-1 : mLastSelected-1;
    }else if(key == GLFW_KEY_DOWN){
        if(mLastSelected == count()-1) return true;
        new_sel = mLastSelected < 0 ? 0 : mLastSelected+1;
    }
    deselectAll();
    auto pli = getItem<ListBase::ListItemBase>(new_sel);
    if(pli){
        if((mFlags & Multiple) and (modifiers & GLFW_MOD_SHIFT))//range selection; takes priority
            selectRange(new_sel);
        else{
            setSelection(new_sel);
            if(mCallback)
                mCallback(new_sel,pli->selected());
        }
        //try to scroll patient if necessary (i.e. list embedded in a vscroll panel)
        int dy = std::min(0,(mPos+pli->position()).y());
        if(dy ==0)
            dy = std::max(0,(mPos+pli->position()+pli->size()-parent()->size()).y());
        if(parent()){
            parent()->scrollEvent(Vector2i(), Vector2f(0.0f,dy));//scroll up(-) or down(+)
        }
    }
    return true;
}


/***/
void ListBase::addChild(int index, Widget * widget) {
    if(dynamic_cast<ListBase::ListItemBase*>(widget)){
        Widget::addChild(index, widget);
        if(mFlags & Sorted){
            sort();
        }
    }
}

void ListBase::sort(){
    std::stable_sort(mChildren.begin(),mChildren.end(),sortByTag);
}

void ListBase::save(Serializer &s) const {
    Widget::save(s);
    std::vector<int> selrange;
    getSelection(selrange);
    s.set("flags", mFlags);
    s.set("selection",selrange);
    s.set("last_selection", mLastSelected);
}

bool ListBase::load(Serializer &s) {
    std::vector<int> selrange;
    if (!Widget::load(s)) return false;
    if (!s.get("flags", mFlags)) return false;
    if (!s.get("selection",selrange)) return false;
    if (!s.get("last_selection", mLastSelected)) return false;

    for(auto i : selrange){
         dynamic_cast<ListBase::ListItemBase*>(mChildren[i])->select();
    }
    return true;
}

NAMESPACE_END(nanogui)

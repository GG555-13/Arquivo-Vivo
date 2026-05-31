#include "TabGroup.h"

#include "Draggable.h"
#include "GameObject.h"
#include "Interactable.h"
#include "SpriteRenderer.h"
#include "State.h"

TabGroup::TabGroup(State& owner) : state(owner), activeIndex(-1)
{
}

int TabGroup::AddTab(Vec2 buttonCenter,
                     const std::string& downSprite, const std::string& upSprite)
{
    int index = static_cast<int>(tabs.size());
    TabEntry entry;
    entry.buttonCenter = buttonCenter;

    GameObject* downBtn = new GameObject();
    SpriteRenderer* downSR = new SpriteRenderer(*downBtn, downSprite);
    downSR->SetScale(0.56782f, 0.56782f);
    downBtn->AddComponent(downSR);
    downBtn->box.SetCenter(buttonCenter);
    downBtn->AddComponent(new Interactable(
        *downBtn,
        Interactable::CLICK_ONLY,
        Interactable::NO_ACTOR,
        0.0f,
        [this, index]() { SwitchTo(index); }
    ));
    entry.buttonDown = state.AddObject(downBtn);

    GameObject* upBtn = new GameObject();
    SpriteRenderer* upSR = new SpriteRenderer(*upBtn, upSprite);
    upSR->SetScale(0.56782f, 0.56782f);
    upBtn->AddComponent(upSR);
    upBtn->box.SetCenter(Vec2(buttonCenter.x, buttonCenter.y + OFFSCREEN_Y));
    upBtn->AddComponent(new Interactable(
        *upBtn,
        Interactable::CLICK_ONLY,
        Interactable::NO_ACTOR,
        0.0f,
        []() { /* no-op */ }
    ));
    entry.buttonUp = state.AddObject(upBtn);

    tabs.push_back(entry);
    return index;
}

void TabGroup::AddContent(int tabIndex, std::weak_ptr<GameObject> obj)
{
    if (tabIndex < 0 || tabIndex >= static_cast<int>(tabs.size()))
        return;
    tabs[tabIndex].contents.push_back(obj);
}

void TabGroup::SwitchTo(int index)
{
    if (index < 0 || index >= static_cast<int>(tabs.size()) || index == activeIndex)
        return;

    // sair da aba atual
    if (activeIndex >= 0)
    {
        TabEntry& oldTab = tabs[activeIndex];
        CancelDraggables(oldTab);

        for (auto& wp : oldTab.contents)
        {
            if (auto obj = wp.lock())
                obj->box.y += OFFSCREEN_Y;
        }

        if (auto downBtn = oldTab.buttonDown.lock())
            downBtn->box.SetCenter(oldTab.buttonCenter);
        if (auto upBtn = oldTab.buttonUp.lock())
            upBtn->box.SetCenter(Vec2(oldTab.buttonCenter.x,
                                      oldTab.buttonCenter.y + OFFSCREEN_Y));
    }

    // entrar na nova aba
    TabEntry& newTab = tabs[index];

    if (auto downBtn = newTab.buttonDown.lock())
        downBtn->box.SetCenter(Vec2(newTab.buttonCenter.x,
                                    newTab.buttonCenter.y + OFFSCREEN_Y));
    if (auto upBtn = newTab.buttonUp.lock())
        upBtn->box.SetCenter(newTab.buttonCenter);

    for (auto& wp : newTab.contents)
    {
        if (auto obj = wp.lock())
            obj->box.y -= OFFSCREEN_Y;
    }

    activeIndex = index;
}

int TabGroup::GetActiveIndex() const
{
    return activeIndex;
}

void TabGroup::CancelDraggables(TabEntry& tab)
{
    for (auto& wp : tab.contents)
    {
        if (auto obj = wp.lock())
        {
            Draggable* d = obj->GetComponent<Draggable>();
            if (d && d->IsDragging())
                d->ResetToSpawn();
        }
    }
}

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include "SnapMath.hpp"

using namespace geode::prelude;

// Thin wrapper so the rest of the file doesn't need to know Mod::get()
// settings-key strings scattered everywhere.
namespace settings {
    bool enabled()      { return Mod::get()->getSettingValue<bool>("enabled"); }
    float distance()    { return Mod::get()->getSettingValue<float>("snap-distance"); }
    bool snapCenters()  { return Mod::get()->getSettingValue<bool>("snap-to-centers"); }
    bool showGuides()   { return Mod::get()->getSettingValue<bool>("show-guides"); }
}

// Draws the temporary alignment line(s) while a snap is active. One node,
// reused every frame, cleared each time so we're not leaking CCDrawNodes.
class SnapGuideNode : public CCDrawNode {
public:
    static SnapGuideNode* get(CCNode* editorLayer) {
        static constexpr int kTag = 0x534e4150; // "SNAP" in hex, arbitrary unique tag
        auto existing = static_cast<SnapGuideNode*>(editorLayer->getChildByTag(kTag));
        if (existing) return existing;

        auto node = SnapGuideNode::create();
        node->setTag(kTag);
        node->setZOrder(1000); // draw above objects
        editorLayer->addChild(node);
        return node;
    }

    void showVertical(float x, CCSize const& winSize) {
        clear();
        drawSegment({x, 0}, {x, winSize.height}, 1.f, {1.f, 0.35f, 0.6f, 1.f});
    }

    void showHorizontal(float y, CCSize const& winSize) {
        // NOTE: if both axes snap in the same frame you'll want to accumulate
        // instead of clearing here — left as a follow-up once the basic
        // version is working, so you can see each axis independently first.
        clear();
        drawSegment({0, y}, {winSize.width, y}, 1.f, {1.f, 0.35f, 0.6f, 1.f});
    }

    void hide() {
        clear();
    }
};

class $modify(SmartSnapEditorUI, EditorUI) {
    // Gathers nearby objects' bounding boxes, excluding the object(s)
    // currently being dragged. Restrict to the current + adjacent sections
    // for performance rather than scanning the whole level.
    std::vector<CCRect> collectNearbyBoxes(GameObject* dragged) {
        std::vector<CCRect> boxes;

        auto lel = LevelEditorLayer::get();
        if (!lel) return boxes;

        // TODO(verify against your Geode SDK version):
        // LevelEditorLayer exposes the currently loaded/active objects as
        // some flavor of CCArray (commonly `m_objects` or reachable via a
        // helper like `getObjectsInSections`/similar depending on SDK
        // version). Run `geode binding list LevelEditorLayer` from your
        // Geode CLI, or check GeneratedSource/2.2074/LevelEditorLayer.hpp
        // in your local Geode SDK install, and swap this loop to use
        // whichever accessor is current. Pseudocode intent below:
        //
        // for (auto obj : CCArrayExt<GameObject*>(lel->m_objects)) {
        //     if (obj == dragged) continue;
        //     if (!obj->m_isSelected) { // skip other selected objects too, optional
        //         boxes.push_back(obj->boundingBox());
        //     }
        // }

        return boxes;
    }

    // TODO(verify against your Geode SDK version):
    // This is the one hook you need to confirm locally. You're looking for
    // whatever EditorUI (or GameObject, depending on how RobTop structured
    // drag-move in the current GD build) calls on every frame while an
    // object is being actively dragged with the mouse/touch — NOT the
    // keyboard-nudge function (that one's usually called something like
    // `moveObject` and only fires once per keypress, which won't give you
    // live guide lines while dragging).
    //
    // How to find it:
    //   1. `geode binding list EditorUI` (or GameObject) in your Geode CLI,
    //      grep for anything mentioning "drag", "move", or "touch".
    //   2. Or check the Geode Discord's #gd-modding-help / SDK docs for
    //      "editor drag hook" — this comes up often enough that someone's
    //      likely posted the current answer for this GD version.
    //   3. Once found, replace the placeholder below with the real
    //      signature and call site.
    //
    // Below is the shape the implementation should take once you have it:

    /*
    void onDragObject(GameObject* obj, CCPoint& proposedPos) {
        EditorUI::onDragObject(obj, proposedPos); // call through first

        if (!settings::enabled()) return;

        // Optional: hold a key to bypass snapping for precise placement
        if (CCKeyboardDispatcher::get()->getShiftKeyPressed()) return;

        auto nearby = collectNearbyBoxes(obj);
        auto draggedBox = obj->boundingBox();

        auto ownX = collectCandidatesX(draggedBox, settings::snapCenters());
        auto ownY = collectCandidatesY(draggedBox, settings::snapCenters());

        std::vector<SnapCandidate> neighborX, neighborY;
        for (auto& box : nearby) {
            auto cx = collectCandidatesX(box, settings::snapCenters());
            auto cy = collectCandidatesY(box, settings::snapCenters());
            neighborX.insert(neighborX.end(), cx.begin(), cx.end());
            neighborY.insert(neighborY.end(), cy.begin(), cy.end());
        }

        auto resultX = findBestSnap(ownX, neighborX, settings::distance());
        auto resultY = findBestSnap(ownY, neighborY, settings::distance());

        if (resultX.snapped) proposedPos.x += resultX.snappedValue;
        if (resultY.snapped) proposedPos.y += resultY.snappedValue;

        if (settings::showGuides()) {
            auto guide = SnapGuideNode::get(this);
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            if (resultX.snapped) guide->showVertical(resultX.guideLineCoord, winSize);
            else if (resultY.snapped) guide->showHorizontal(resultY.guideLineCoord, winSize);
            else guide->hide();
        }
    }
    */
};

$on_mod(Loaded) {
    log::info("Smart Snap loaded");
}

#pragma once

#include <Geode/Geode.hpp>
#include <optional>
#include <vector>

using namespace geode::prelude;

// One candidate alignment line found on a nearby object, along one axis.
struct SnapCandidate {
    float value;     // the x (or y) coordinate this line sits at, in editor space
    bool isCenter;    // true = object's center line, false = an edge
};

// Result of running the snap search on one axis.
struct SnapResult {
    bool snapped = false;
    float snappedValue = 0.f;
    float guideLineCoord = 0.f; // where to draw the guide (same as snappedValue here,
                                 // kept separate in case you want to offset it later)
};

// Collects the candidate snap lines (left/right/center-x or bottom/top/center-y)
// for a single object's bounding box, on one axis.
inline std::vector<SnapCandidate> collectCandidatesX(CCRect const& box, bool includeCenter) {
    std::vector<SnapCandidate> out;
    out.push_back({box.getMinX(), false});
    out.push_back({box.getMaxX(), false});
    if (includeCenter) {
        out.push_back({box.getMidX(), true});
    }
    return out;
}

inline std::vector<SnapCandidate> collectCandidatesY(CCRect const& box, bool includeCenter) {
    std::vector<SnapCandidate> out;
    out.push_back({box.getMinY(), false});
    out.push_back({box.getMaxY(), false});
    if (includeCenter) {
        out.push_back({box.getMidY(), true});
    }
    return out;
}

// Given the dragged object's own candidate lines (its left/right/center, or
// top/bottom/center) and a flat list of candidate lines gathered from nearby
// objects, find the smallest-distance match within threshold, and report how
// far the dragged object needs to shift to land exactly on it.
inline SnapResult findBestSnap(
    std::vector<SnapCandidate> const& ownLines,
    std::vector<SnapCandidate> const& neighborLines,
    float threshold
) {
    SnapResult best;
    float bestDist = threshold;

    for (auto const& own : ownLines) {
        for (auto const& other : neighborLines) {
            float dist = std::fabs(own.value - other.value);
            if (dist < bestDist) {
                bestDist = dist;
                best.snapped = true;
                // Shift = how much to move the dragged object so `own.value`
                // lands exactly on `other.value`.
                best.snappedValue = other.value - own.value;
                best.guideLineCoord = other.value;
            }
        }
    }

    return best;
}

#ifndef PLUTOBOOK_LISTITEMBOX_H
#define PLUTOBOOK_LISTITEMBOX_H

#include "blockbox.h"
#include "inlinebox.h"

namespace plutobook {

class ListItemBox final : public BlockFlowBox {
public:
    ListItemBox(Node* node, const RefPtr<BoxStyle>& style);

    bool isListItemBox() const final { return true; }

    const char* name() const final { return "ListItemBox"; }
};

template<>
struct is_a<ListItemBox> {
    static bool check(const Box& box) { return box.isListItemBox(); }
};

class InsideListMarkerBox final : public InlineBox {
public:
    InsideListMarkerBox(const RefPtr<BoxStyle>& style);

    bool isInsideListMarkerBox() const final { return true; }

    const char* name() const final { return "InsideListMarkerBox"; }
};

template<>
struct is_a<InsideListMarkerBox> {
    static bool check(const Box& box) { return box.isInsideListMarkerBox(); }
};

class OutsideListMarkerBox final : public BlockFlowBox {
public:
    OutsideListMarkerBox(const RefPtr<BoxStyle>& style);

    bool isOutsideListMarkerBox() const final { return true; }

    const char* name() const final { return "OutsideListMarkerBox"; }
};

template<>
struct is_a<OutsideListMarkerBox> {
    static bool check(const Box& box) { return box.isOutsideListMarkerBox(); }
};

} // namespace plutobook

#endif // PLUTOBOOK_LISTITEMBOX_H

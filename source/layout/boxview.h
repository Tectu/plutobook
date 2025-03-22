#ifndef PLUTOBOOK_BOXVIEW_H
#define PLUTOBOOK_BOXVIEW_H

#include "blockbox.h"

namespace plutobook {

class PageBox;

class BoxView final : public BlockFlowBox {
public:
    BoxView(Document* document, const RefPtr<BoxStyle>& style);

    bool isBoxView() const final { return true; }
    bool requiresLayer() const final { return true; }
    BoxStyle* backgroundStyle() const { return m_backgroundStyle; }

    Rect backgroundRect() const;

    void setCurrentPage(PageBox* page) { m_currentPage = page; }
    PageBox* currentPage() const { return m_currentPage; }

    void computeWidth(float& x, float& width, float& marginLeft, float& marginRight) const final;
    void computeHeight(float& y, float& height, float& marginTop, float& marginBottom) const final;
    void layout(FragmentBuilder* fragmentainer) final;
    void build() final;

    const char* name() const final { return "BoxView"; }

private:
    BoxStyle* m_backgroundStyle{nullptr};
    PageBox* m_currentPage{nullptr};
};

template<>
struct is_a<BoxView> {
    static bool check(const Box& box) { return box.isBoxView(); }
};

} // namespace plutobook

#endif // PLUTOBOOK_BOXVIEW_H

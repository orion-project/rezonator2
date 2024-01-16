#ifndef SCHEMA_LAYOUT_DEFS_H
#define SCHEMA_LAYOUT_DEFS_H

#define LAYOUT_BEGIN \
class Layout : public ElementLayout { \
        public: \
        Layout(Element *elem) : ElementLayout(elem) {}
#define INIT void init() override
#define PAINT void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override
#define OVERRIDE_SELECTED(target) \
    void setSelected(bool on) override { ElementLayout::setSelected(on); target->setSelected(on); }
#define LAYOUT_END };

#define OPTIONS_BEGIN \
class LayoutOptions : public ElementLayoutOptionsView {
#define HAS_ALT_VERSION bool hasAltVersion() const override { return true; }
#define ALT_VERSION_OPTION_TITLE(title) const char* altVersionOptionTitle() const override { return title; }
#define OPTIONS_END };

#endif // SCHEMA_LAYOUT_DEFS_H

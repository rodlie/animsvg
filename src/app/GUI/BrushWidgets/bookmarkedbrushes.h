#ifndef BOOKMARKEDBRUSHES_H
#define BOOKMARKEDBRUSHES_H

#include "Paint/brushescontext.h"
#include "brushwidget.h"

#include "../bookmarkedwidget.h"

class BookmarkedBrushes : public BookmarkedWidget {
    Q_OBJECT
public:
    BookmarkedBrushes(const bool vertical,
                      const int dimension,
                      BrushesContext* const brushesContext,
                      QWidget* const parent);
private:
    BrushesContext* const mBrushesContext;
};

#endif // BOOKMARKEDBRUSHES_H

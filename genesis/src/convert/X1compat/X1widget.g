//genesis
//
// Make an x1<widget> version from the corresponding Xodus 2 widget.
//
// Expect: only script arg should be the type of widget (minus the x)
//         (e.g. for the xbutton should call "X1widget button")
//

str widgetName = $1
if (widgetName == "")
    echo X1widget: widget name not specified (this is a programming error)
    return
end

create x{widgetName} {widgetName}

    // Here we could add generic widget fields and stuff if necessary

    // these are field name aliases for widget width and height.  This
    // isn't a complete solution since the old width and height do not
    // include widget and edge references while wgeom and hgeom do.  What
    // we do here will work so long as the caller doesn't need to preserve
    // widget and edge refs.

    addfield {widgetName} width -indirect . wgeom
    addfield {widgetName} height -indirect . hgeom

    addclass {widgetName} output

addobject x1{widgetName} {widgetName}

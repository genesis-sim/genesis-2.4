/*
 * FILE: text.c
 *
 */

#include "xplot_ext.h"

/* Forward declarations. */

void
DisplayLabels(BasicWindow *basic);


void
RefreshText(TextWindow *text)
{
    ClearWindow((BasicWindow *)text);
    DisplayLabels((BasicWindow *)text);
}


void
DisplayLabels(BasicWindow *basic)
{
    int sx, sy, sr = 0;
    int sx2, sy2;
    Label *label;

    for (label = basic->label; label; label = label->next)
    {
        /*
         * set the color
         */

        SetColor(label->color);

        /*
         * put up the label data
         */

        switch (label->type)
        {
        case STRING_LBL:
            /*
             * get the coords
             */

            switch (label->coord_mode)
            {
            case WORLD_LBL: /* world coord */
                /* FIXME: shouldn't upcast basic to Graph... */
                ScreenTransform((Graph *)basic, label->u.string.wx,
                                label->u.string.wy, &sx, &sy);
                break;

            case SCREEN_LBL:    /* screen coord */
                sx = label->u.string.sx;
                sy = label->u.string.sy;
                break;

            case WINDOW_LBL:    /* window coord */
                sx = label->u.string.wx * basic->wwidth;
                sy = label->u.string.wy * basic->wheight;
                break;
            }

            /*
             * draw the text
             */

            Text(basic, sx, sy, label->u.string.line);
            break;


        case BOX_LBL:
            /*
             * get the coords
             */

            switch (label->coord_mode)
            {
            case WORLD_LBL: /* world coord */
                ScreenTransform((Graph *)basic,
                                label->u.box.wx, label->u.box.wy, &sx, &sy);
                sr = label->u.box.wr;
                break;

            case SCREEN_LBL:    /* screen coord */
                sx = label->u.box.sx;
                sy = label->u.box.sy;
                sr = label->u.box.sr;
                break;

            case WINDOW_LBL:    /* window coord */
                sx = label->u.box.wx * basic->wwidth;
                sy = label->u.box.wy * basic->wheight;
                sr = label->u.box.wr * basic->wwidth;
                break;
            }

            /*
             * draw the box
             */

            Box(sx - sr, sy - sr, sx + sr, sy + sr);
            break;

        case LINE_LBL:
            /*
             * get the coords
             */

            switch (label->coord_mode)
            {
            case WORLD_LBL: /* world coord */
                ScreenTransform((Graph *)basic, label->u.line.wx,
                                label->u.line.wy, &sx, &sy);
                ScreenTransform((Graph *)basic, label->u.line.wx2,
                                label->u.line.wy2, &sx2, &sy2);
                break;

            case SCREEN_LBL:    /* screen coord */
                sx = label->u.line.sx;
                sy = label->u.line.sy;
                sx2 = label->u.line.sx2;
                sy2 = label->u.line.sy2;
                break;

            case WINDOW_LBL:    /* window coord */
                sx = label->u.line.wx * basic->wwidth;
                sy = label->u.line.wy * basic->wheight;
                sx2 = label->u.line.wx2 * basic->wwidth;
                sy2 = label->u.line.wy2 * basic->wheight;
                break;
            }

            /*
             * draw the line
             */

            DrawLine(sx, sy, sx2, sy2);
            break;
        }
    }
}


/*
 * append a label to the window
 */

void
AddLabelString(BasicWindow *basic, char *s,
               short int sx, short int sy,
               float wx, float wy,
               short int coord_mode, short int priority)
{
    Label *newlabel;

    newlabel = (Label *) calloc(1, sizeof(Label));
    newlabel->type = STRING_LBL;
    newlabel->priority = priority;

    /*
     * assign the color
     */

    newlabel->color = basic->foreground;

    /*
     * get the label coordinates
     */

    newlabel->coord_mode = coord_mode;

    switch (coord_mode)
    {
    case WORLD_LBL:     /* world coords */
        newlabel->u.string.wx = wx;
        newlabel->u.string.wy = wy;
        break;

    case SCREEN_LBL:        /* screen coords */
        newlabel->u.string.sx = sx;
        newlabel->u.string.sy = sy;
        break;

    case WINDOW_LBL:        /* screen coords */
        newlabel->u.string.wx = (float) sx / basic->wwidth;
        newlabel->u.string.wy = (float) sy / basic->wheight;
        break;
    }

    /*
     * get the string
     */

    strncpy(newlabel->u.string.line, s, MAX_TEXT_LINE);

    /* just to make sure it is NULL terminated */

    newlabel->u.string.line[MAX_TEXT_LINE - 1] = '\0';

    /*
     * insert it into the list
     */

    newlabel->next = basic->label;
    basic->label = newlabel;
}


/*
 * frees all labels
 */

void
FreeAllLabels(BasicWindow *basic)
{
    Label *next;

    while (basic->label)
    {
        next = basic->label->next;
        free(basic->label);
        basic->label = next;
    }
}


/*
 * frees temporary labels
 */

void
FreeLabels(BasicWindow *basic)
{
    Label *next;
    Label *current;
    Label *prev;

    prev = NULL;
    current = basic->label;

    while (current)
    {
        next = current->next;

        if (current->priority == TEMPORARY_LBL)
        {
            free(current);

            if (prev == NULL)
            {
                basic->label = next;
            }
            else
            {
                prev->next = next;
            }
        }
        else
        {
            prev = current;
        }
        current = next;
    }
}


/*
 * append a line label to the window
 */

void
AddLabelLine(BasicWindow *basic, short int sx, short int sy,
             short int sy2, short int sx2,
             float wx, float wy, float wx2, float wy2,
             short int coord_mode, short int priority)
{
    Label *newlabel;

    newlabel = (Label *) calloc(1, sizeof(Label));
    newlabel->type = LINE_LBL;
    newlabel->priority = priority;

    /*
     * assign the color
     */

    newlabel->color = basic->foreground;

    /*
     * get the label coordinates
     */

    newlabel->coord_mode = coord_mode;

    switch (coord_mode)
    {
    case WORLD_LBL:     /* world coords */
        newlabel->u.line.wx = wx;
        newlabel->u.line.wy = wy;
        newlabel->u.line.wx2 = wx2;
        newlabel->u.line.wy2 = wy2;
        break;

    case SCREEN_LBL:        /* screen coords */
        newlabel->u.line.sx = sx;
        newlabel->u.line.sy = sy;
        newlabel->u.line.sx2 = sx2;
        newlabel->u.line.sy2 = sy2;
        break;

    case WINDOW_LBL:        /* window coords */
        newlabel->u.line.wx = (float) sx / basic->wwidth;
        newlabel->u.line.wy = (float) sy / basic->wheight;
        newlabel->u.line.wx = (float) sx2 / basic->wwidth;
        newlabel->u.line.wy = (float) sy2 / basic->wheight;
        break;
    }

    /*
     * insert it into the list
     */

    newlabel->next = basic->label;
    basic->label = newlabel;
}


/*
 * append a label to the window
 */

void
AddLabelBox(BasicWindow *basic, short int sx, short int sy,
            short int sr, float wx, float wy, float wr,
            short int coord_mode, short int priority)
{
    Label *newlabel;

    newlabel = (Label *) calloc(1, sizeof(Label));
    newlabel->type = BOX_LBL;
    newlabel->priority = priority;

    /*
     * assign the color
     */

    newlabel->color = basic->foreground;

    /*
     * get the label coordinates
     */

    newlabel->coord_mode = coord_mode;

    switch (coord_mode)
    {
    case WORLD_LBL:     /* world coords */
        newlabel->u.box.wx = wx;
        newlabel->u.box.wy = wy;
        newlabel->u.box.wr = wr;
        break;

    case SCREEN_LBL:        /* screen coords */
        newlabel->u.box.sx = sx;
        newlabel->u.box.sy = sy;
        newlabel->u.box.sr = sr;
        break;

    case WINDOW_LBL:        /* window coords */
        newlabel->u.box.wx = (float) sx / basic->wwidth;
        newlabel->u.box.wy = (float) sy / basic->wheight;
        newlabel->u.box.wr = (float) sr / basic->wwidth;
        break;
    }

    /*
     * insert it into the list
     */

    newlabel->next = basic->label;
    basic->label = newlabel;
}


Label *
FindLabel(BasicWindow *basic, short int cx, short int cy)
{
    Label *label;
    Label *current;
    int distance = -1;
    int ds;
    int sx, sy, sr;

    current = NULL;

    for (label = basic->label; label; label = label->next)
    {
        switch (label->type)
        {
        case STRING_LBL:
            /*
             * get the coords
             */

            switch (label->coord_mode)
            {
            case WORLD_LBL: /* world coord */
                ScreenTransform((Graph *)basic, label->u.string.wx,
                                label->u.string.wy, &sx, &sy);
                break;

            case SCREEN_LBL:    /* screen coord */
                sx = label->u.string.sx;
                sy = label->u.string.sy;
                break;

            case WINDOW_LBL:    /* window coord */
                sx = label->u.string.wx * basic->wwidth;
                sy = label->u.string.wy * basic->wheight;
                break;
            }

            break;

        case BOX_LBL:
            /*
             * get the coords
             */

            switch (label->coord_mode)
            {
            case WORLD_LBL: /* world coord */
                ScreenTransform((Graph *)basic,
                                label->u.box.wx, label->u.box.wy, &sx, &sy);
                sr = label->u.box.wr;
                break;

            case SCREEN_LBL:    /* screen coord */
                sx = label->u.box.sx;
                sy = label->u.box.sy;
                sr = label->u.box.sr;
                break;

            case WINDOW_LBL:    /* window coord */
                sx = label->u.box.wx * basic->wwidth;
                sy = label->u.box.wy * basic->wheight;
                sr = label->u.box.wr * basic->wwidth;
                break;
            }

            break;


        case LINE_LBL:
            /*
             * get the coords
             */

            switch (label->coord_mode)
            {
            case WORLD_LBL: /* world coord */
                ScreenTransform((Graph *)basic,
                                label->u.line.wx, label->u.line.wy, &sx, &sy);
                break;

            case SCREEN_LBL:    /* screen coord */
                sx = label->u.line.sx;
                sy = label->u.line.sy;
                break;

            case WINDOW_LBL:    /* window coord */
                sx = label->u.line.wx * basic->wwidth;
                sy = label->u.line.wy * basic->wheight;
                break;
            }

            break;
        }

        ds = sqrt((double) ((sx - cx) * (sx - cx) + (sy - cy) * (sy - cy)));

        if (ds < distance || distance == -1)
        {
            distance = ds;
            current = label;
        }
    }

    return current;
}


void
DeleteLabel(BasicWindow *basic, Label *label)
{
    Label *l;

    if (label == NULL)
        return;

    if (label == basic->label)
    {
        basic->label = basic->label->next;
        return;
    }

    for (l = basic->label; l; l = l->next)
    {
        if (label == l->next)
        {
            l->next = l->next->next;
        }
    }
}

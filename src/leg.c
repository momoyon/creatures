#include <leg.h>

Leg make_leg(Vector2 pos, float length) {
    Leg l = {0};

    l.start = pos;

    l.segments[0].length = length;
    l.segments[1].length = length * 0.75f;
    l.segments[2].length = length * 0.25f;

    return l;
}

void update_leg(Leg *l) {
    Segment *one = &(l->segments[0]);
    Segment *two = &(l->segments[1]);
    Segment *thr = &(l->segments[2]);

    one->start = l->start;
    segment_end_to_start(one);

    two->start = one->end;
    segment_end_to_start(two);

    thr->start = two->end;
    segment_end_to_start(thr);

    l->end = thr->end;
}

void draw_leg(Leg *l, bool debug) {
    for (int i = 0; i < 3; ++i) {
        Segment *s = &l->segments[i];
        draw_segment(s, debug);
    }
}

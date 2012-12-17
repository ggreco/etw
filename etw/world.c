#include "eat.h"

void add_achievement(const char *name, float progress)
{
    D(bug("Adding to achievement %s progress %f\n", name, progress));
}

void add_score(int score)
{
    D(bug("Adding global score %d", score));
}

void show_world_scores()
{
    D(bug("TODO world score\n"));
}

void show_achievements()
{
    D(bug("TODO show achievements\n"));
}

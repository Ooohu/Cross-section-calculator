struct coordinate {
    double x;
    double y;
    struct coordinate *next;
};

struct coordinate *prepend(struct coordinate *list, double x,
    double y);

void clean(struct coordinate *list);

double x (double s, double t);

double y (double s, double t);

double z (double s, double t);

void plot (struct coordinate *list, double a1, double a2, double r, double v, char* folder, char *f
	, int count);


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

double area(double x, double y);

double distance (double x, double y, double z, double x0, double y0, 
		double z0);

double ball(double z0, double v, double t);

void plot (struct coordinate *list, double a1, double a2, double r,char *f);

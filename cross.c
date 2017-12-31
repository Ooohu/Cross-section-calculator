#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include"cross.h"

/**WAVE FUNCTION (Remember to put semicolon ; at the end of the function.*/

#define a0 0.8	    /*set the constant value.*/

double x (double s, double t){	/*x components of the dynamic equation*/
    double x =1/2*((1-a0)*sin(s-t)+1/3*a0*sin(3*(s-t))+sin(s+t))
    ;return x;
}

double y (double s, double t){  /*y components of the dynamic equation*/
    double y =-1/2*((1-a0)*cos(s-t)+1/3*a0*cos(3*(s-t))+cos(s+t))  
    ;return y;
}

double z (double s, double t){  /*z components of the dynamic equation*/
    double z =-a0*sqrt(1-a0)*cos(s-t)
    ;return z;
}

/***************END OF THE WAVE FUNCTION**********************/

int main()
{

/**************************Configuration**********************/
    
    char filename[]= "Plot1"; /*"Name" of the output plot.*/

    double length = 2*acos(-1.0);//length of the string: 2\pi.

    /*Parameters of the ball*/
    double r = 0.04;	//radius.
    double z0 = 0.5;	//height.
    double v = 0.01;	//velocity.

    /*Parameters of the test area*/
    double a1 = 0.2;	//x coordinate of the ball from -a1 to a1. 
    double a2 = 0.2;	//y coordinate of the ball from -a2 to a2.

    /*Setps sizes*/
    double time = 1;        //time step dt.
    double string_element = 0.1; 
    double axis_unit = 0.02;//spatial steps dx, dy, ds.


/***********************End of Configuration******************/

   //initializ the x coordinate of the ball.

    int in  = 0; //Initialization of counters in & out.
    int out = 0; 
    double area = 4*a1*a2;//area of the test area.

    struct coordinate *list = NULL;//Empty container for storing coordinates.
    printf("Calculating...\n");

    /*1. "area" loop.(i,j for x,y coordinate, j for y coordinate; 
     *  inside 2a1 X 2a2)
     */
    for (double x0 = -a1; x0 <= a1; x0+=axis_unit) 
    {


	for(double y0 = -a2; y0 <= a2; y0+=axis_unit){

	    //2. line element (eta) loop (from 0 to length).
	    for (double s=0; s<=length; s+=(string_element)/length){ 

		//3. time loop.		
		double height = z0;//z coordinates of the ball (from z0 to -z0)
		for (double t=0; height > (-z0); t+=time){

		    height = ball(z0,v,t);
		    int mark = in;//if the "mark" is unchanged, it means no collision.
		    double dis =distance(x(s,t) ,y(s,t) ,z(s,t) ,x0 ,y0 ,height );
		    if ( dis <= r)//collision.
		    {	
			list = prepend(list, x0, y0);
			in++;
			height = (-z0);//manually ends the time loop
		    }   
		    if (in == mark && height <=(-z0)) {//no collision.
			out++;
		    }
		}
	    }
	}
    }
    printf("Counting results: %d balls intersect, %d balls miss.\n", in, out);
    printf("The cross-section is %g.\n", area*in/(in+out));
    if (in == 0 || out ==0) 
    { 
    printf("No plot availabe, because no balls %s.\n", (in ==0)? "intersect":  "miss");
    clean(list);//clean up data.
    return 0; 
    }
    
    plot( list, a1, a2, r, filename);

    clean(list);
}


/*calculate the distance between a string element at (x,y,z) and 
 * a ball at (x0,y0,z0)
 */
double distance (double x, double y, double z, double x0, double y0, 
	double z0){
    /*x,y,z are coordinates of the strings,
     *x0,y0,z0 are coordinates of the ball.
     */
    return sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0)+(z-z0)*(z-z0));
}

/*dynamic equation of the ball: z = -v*t+z0*/
double ball(double z0, double v, double t){ 
    return -v*t+z0;
}


/*plotting function basic on Gnuplot program*/
void plot (struct coordinate *list, double a1, double a2, double r, char *f){ 
    //f is the file name.

    FILE *gnuplot = popen("gnuplot", "w");
    fprintf(gnuplot, "set terminal png\n");
    fprintf(gnuplot, "set output '%s.png'\n",f);
    fprintf(gnuplot, "set title 'Balls collide with the string'\n");
    //Title  is in '  '.
    fprintf(gnuplot, "set style circle radius %g\n",r); //radius of points
    fprintf(gnuplot, "plot '-' with circles lc rgb 'white' fs transparent solid 0 noborder , '-' with circles lc rgb 'red' fs transparent solid 0.1 noborder\n" );//points style
    fprintf(gnuplot, "%g %g\n%g %g\n", a1, a2, -a1, -a2);//to avoid the error "empty range" in GNUPLOT
    fprintf(gnuplot,"e\n");
    while (list)
    {
	fprintf(gnuplot, "%g %g\n", list->x, list->y);
	list = list->next;
    }
    fflush(gnuplot);
    fprintf(gnuplot, "e\n");
}


struct coordinate *prepend(struct coordinate *list, double x,
    double y)
{
    struct coordinate *node = (struct coordinate*)malloc(sizeof(struct coordinate));
    if (node == NULL)
	return NULL;

    node->x = x;
    node->y = y;

    node->next = list;

    return node;
}

void clean(struct coordinate *list){
    if (list){
	clean(list->next);
	free(list);
    }
}


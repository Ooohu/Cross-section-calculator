#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h> //mkdir with 0777 "open to all" perission

#include"cross.h"

/**WAVE FUNCTION (Remember to put semicolon ; at the end of the function.*/

#define a0 0.4	    /*set the constant value.*/

double x (double s, double t){	/*x components of the dynamic equation*/
    double x =1/2*((1-a0)*sin(s-t) + 1/3*a0*sin(3*(s-t)) + sin(s+t))
    ;return x;
}

double y (double s, double t){  /*y components of the dynamic equation*/
    double y =-1/2*((1-a0)*cos(s-t) + 1/3*a0*cos(3*(s-t)) + cos(s+t))  
    ;return y;
}

double z (double s, double t){  /*z components of the dynamic equation*/
    double z =-sqrt(a0*(1-a0))*cos(s-t)
    ;return z;
}

/***************END OF THE WAVE FUNCTION**********************/

int main()
{

/**************************Configuration**********************/
    char folder[]="test6";
    char filename[]= "Plot"; /*"Name" of the output plot.*/

    double length = 2*acos(-1.0);//length of the string: 2\pi.

    /*Parameters of the ball*/
    double rmin=0.01;
    double rmax=0.8;
    double rstep=0.01;
//    double r = 0.06;	//radius.
    double vmin=0.12;
    double vmax=0.12;
    double vstep=0.02;

//    double v = 0.06;	//velocity.
    double z0 = 1.2;	//height.

    /*Parameters of the test area*/
    double a1 = 4;	//x coordinate of the ball from -a1 to a1. 
    double a2 = 4;	//y coordinate of the ball from -a2 to a2.

    /*Setps sizes*/
    double time = 0.5;        //time step dt.
    double string_element = 0.1; 
    double axis_unit = 0.1;//spatial steps dx, dy, ds.


/***********************End of Configuration******************/

   //initializ the x coordinate of the ball.

    int in  = 0; //Initialization of counters in & out.
    int out = 0; 
    double area = 4*a1*a2;//area of the test area.

    //create csv file for recording:
    printf("Start computation, and data will be stored in %s.csv.\n", folder);
    FILE *fp;
    char csv_name[strlen(folder)];
    strcpy(csv_name, folder); 
    strcat(csv_name,".csv");
    fp=fopen (csv_name, "w");
    fprintf(fp, "Trials, a, velocity, radius, cross-section");
    //finish creating csv file.
//Add a v/r loop for data collection. count for tracking.
    int count =0;
    double max_count = ((rmax-rmin)/rstep+1)*((vmax-vmin)/vstep+1);
    for (double r=rmin; r<=rmax; r+=rstep){

	for (double v=vmin; v<=vmax; v+=vstep){
	    /*1. "area" loop.
	     *  ball's coordinates (x_0,y_0) inside 2a1 X 2a2
	     */
	    int progress=0;

	    fprintf(stderr,"	Trial %d/%g is in progress (count to 10):", ++count, max_count);

	    struct coordinate *list = NULL;//Empty container for storing coordinates.
	    for (double x0 = -a1; x0 <= a1; x0+=axis_unit) 
	    {
		/*the following "if function" counts the progress:
		 * # of units along x-axis is: (a1+x0)/(axis_unit)
		 * total # of units is :(2*a1)/(axis_unit)
		 * progress is counted by the ratio of them (in the scale of 10). 
		 */
		int current_progress =10*(a1+x0)/(2*a1);
		if(  current_progress == progress) fprintf(stderr,"%d ", ++progress);

		for(double y0 = -a2; y0 <= a2; y0+=axis_unit){

		    //2. line element (eta) loop (from 0 to length).
		    for (double s=0; s<=length; s+=(string_element)/length){ 

			//3. time loop.		
			double height = z0;//z coordinates of the ball (from z0 to -z0)
			for (double t=0; height > -z0; t+=time){

			    height = -v*t+z0;
			    int mark = in;//if the "mark" is unchanged, it means no collision.
			    double dis =sqrt( pow((x(s,t)-x0) , 2 )+pow( (y(s,t)-y0) , 2) 
				    +pow( (z(s,t)-height) , 2 ));
			    if ( dis <= r)//collision.
			    {	
				list = prepend(list, x0, y0);
				//TESTING printf("%g and %g at %g\n",x0,y0,z(s,t));
				in++;
				break;//manually ends the time loop
			    }   
			    if (in == mark && height <=(-z0)) {//no collision.
				out++;
			    }
			}
		    }
		}
	    }

	    printf("\nCounting results: %d balls intersect, %d balls miss.\n", in, out);
	    printf("The cross-section is %g.\n", area*in/(in+out));

	    plot( list, a1, a2, r, v, folder, filename, count);

	    clean(list);

	    printf("Fill in %s with data.\n", csv_name);

	    fprintf(fp, "\n%d,%g,%g,%g,%g", count, a0, v, r, area*in/(in+out));

	}
    }
    fclose(fp);
}


/*plotting function basic on Gnuplot program*/
void plot (struct coordinate *list, double a1, double a2, double r, double v, char *folder, char *f, 
    int count){ 
    //f is the file name.
    
	mkdir(folder,0777);

    FILE *gnuplot = popen("gnuplot", "w");
    fprintf(gnuplot, "set terminal png\n");
    fprintf(gnuplot, "set output './%s/%s%d.png'\n",folder,f,count);
    fprintf(gnuplot, "set title 'Balls collide with the string (a=%g, v=%g, r=%g)'\n", a0, v, r);
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


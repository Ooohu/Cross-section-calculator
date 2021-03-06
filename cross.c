#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h> //mkdir with 0777 "open to all" perission
#include <time.h>//for extracting time info from the system.


/**WAVE FUNCTION (For fraction, integers needs decimal point, e.g. 1->1.0 */

#define a0 0.4	    /*set the constant value.*/

double x (double s, double t){	/*x components of the dynamic equation*/
    double x =(1.0/2)*((1-a0)*sin(s-t) + (1.0/3)*a0*sin(3*(s-t)) + sin(s+t))
    ;return x;
}

double y (double s, double t){  /*y components of the dynamic equation*/
    double y =-(1.0/2)*((1-a0)*cos(s-t) + (1.0/3)*a0*cos(3*(s-t)) + cos(s+t))  
	;return y;
}

double z (double s, double t){  /*z components of the dynamic equation*/
    double z =-sqrt(a0*(1-a0))*cos(s-t)
	;return z;
}

/*************************************************************/
/***************END OF THE WAVE FUNCTION**********************/
/*************************************************************/


int main()
{
    double two_pi = 2*3.142;//it was 2*acos(-1.0);length of the string: 2pi.

/**************************Configuration**********************/
    char folder[]="test";

    /*Parameters of the ball*/
    double rmin=0.06;
    double rmax=0.06;
    double rstep=0.01;

    double vmin=0.9;
    double vmax=0.92;
    double vstep=0.01;

    /*Setps sizes*/		    
    double time_factor = 0.01;//time step = time_factor*r. Initial value is 0.1

    double phase_factor = 0.1;// step of string element = phase_factor*r

   // double axis_factor = 0.1; //The size of a grid is: axis_factor*rmin

    bool detail = false; //set detail equal to 1 to record each collision points.
    bool fit_line = false;//fit the plot with a given funcrion, see the end of file.

    int ball = 30; //number of balls dropped.

/*************************************************************/
/***********************End of Configuration******************/
/*************************************************************/


//0.1 Prepare .csv file for recording:
    printf("Start computation. Data will be stored in %s.csv;"
	    ,folder);
    
    if(detail){ 
	mkdir(folder,0777);
	printf(" Detail of collision can be found in folder: %s"
		,folder);       
    }

    FILE *fp;
    char csv_name[strlen(folder)];
    strcpy(csv_name, folder); 
    strcat(csv_name,".csv");

    //Check existence of experiment; if it is new, then proceed.
    if ( access (csv_name, F_OK) != -1){
	printf("\nAn existing file would be replaced" 
		" unless press Ctrl+c to cancel.\n");
	//sleep(8);
    }

/*    double a1 = 0; 
    double a2 = 0;//a1,a2 label the corner (+|x|,+|y|) of the test region.
    double a3 = 0;
    double a4 = 0;//a3,a4 for corner (-|x|,-|y|)
*/    
    double a1=1;
    double a2=1;
    double z0 = 0; //height of the ball to be determined
    double z9 = 0;


//0.2 Estimate initial start height and size of test region.
		    //make sure s and t can go through a period.
/*    for (double s=0; s<= two_pi+phase_factor*rmin; s+=phase_factor*rmin){ 
	for (double t=0; t<= two_pi+time_factor*rmin; t+=time_factor*rmin){ 
	    if(x(s,t) > a1) a1 = x(s,t);
	    if(y(s,t) > a2) a2 = y(s,t);
	    
	    if(x(s,t) < a3) a3 = x(s,t);
	    if(y(s,t) < a4) a4 = y(s,t);
	    
	    if(z(s,t) > z0) z0 = z(s,t);
	    if(z(s,t) < z9) z9 = z(s,t);
	}
    }
*/    srand(time(NULL) );//make rand() changes as time goes.

//    a1 = ((a1<(-1*a3))? -1*a3:a1)+ rmax*(1 + (double)rand()/(double)RAND_MAX);
//    a2 = ((a2<(-1*a4))? -1*a4:a2)+ rmax*(1 + (double)rand()/(double)RAND_MAX);    
    z0 = ((z0<(-1*z9))? -1*z9:z0)+ rmax*(1 + (double)rand()/(double)RAND_MAX);

    fp=fopen (csv_name, "w");
	//time resolution: time_factor*r
	//string resolution: phase_factor*r
	//grid resolution: axis_factor*r
    fprintf(fp, " Test region:, %g x %g, String length, < %g\n"
	    "Trials, time resolution, string resolution,"
	    "number of balls, parameter a," 
	    "# of balls, velocity, radius, cross-section",
	    2*a1 , 2*a2,
	    (a1<a2)? 6.28*a2:6.28*a1 //circumsphere with radius as a1 or a2.
	    );		 
    fclose(fp);

//0.3 Reject or adjust unreasonable trials:
    if(rmin>0.1*a1 || rmin>0.1*a2){//No large balls r<0.1*r_{test region}
	printf("\nBall is too large (>%g)," 
	    "the experiment is canceled.\n", (a1<a2)?a1*0.1:a2*0.1);
	exit(1);
    }
    if(rmax>0.1*a1 || rmax>0.1*a2){//Adjust rmax: rmax<=0.1*r_{test region} 
	rmax= (a1<a2)? 0.1*a1 : 0.1*a2; 
	printf("\nRmax is re-set to %g, because it is too large.\n", rmax);
    }
    double area = 4*a1*a2;

//0.4 Estimate number of trials
    int count = 1;//tracking the progress of trials
    int max_count=0;
    for (double i = rmin; i <=rmax; i+=rstep){
	for (double i = vmin; i <=vmax; i+=vstep) 
	    max_count++;//number of trials
    }

/***************Trial starts******************/

/**1. Trails through different radiis **/	
    for (double r=rmin; r<=rmax; r+=rstep){ 

	/**Adjust steps**/
//	double axis_unit = axis_factor*rmin;
	double string_unit = phase_factor*r;
	double time_unit = time_factor*r;

	/**Estimate the number of balls (grids)**/
//	int grids = 0;

/*	for (double x0 = -a1; x0 < a1+axis_unit; x0+=axis_unit){ 
	    for(double y0 = -a2; y0 < a2+axis_unit; y0+=axis_unit) 
		grids++;
	}
*/
/**2. Trails through different velocities**/	
	for (double v=vmin; v<=vmax; v+=vstep){
	    
	    fp=fopen(csv_name,"a");
	    
	    time_t rawtime;
	    struct tm * timeinfo;

	    time( &rawtime);
	    timeinfo= localtime ( &rawtime );

	    fprintf(stderr, "\n	Started at: %s", asctime(timeinfo) );

	    int in  = 0; 
	    int out = 0; 
	    int progress=0;

	    fprintf(stderr,"	Test region: %g x %g;" 
		    " Number of balls randomly dropped in test region: %d.\n"
		    "Trial (r=%g, v=%g) %d/%d is in progress (count to 10):",
		    2*a1, 2*a2, ball ,r ,v ,count , max_count);

	/**Prepare data file for recording collision events**/
	    FILE *dp;

	    if(detail){
		char file_name[strlen(folder)+sizeof(count)+20];

		sprintf(file_name,"%s/%d.csv",folder,count);

		dp = fopen(file_name, "w");

		fprintf(dp, "Radius,%g, Velocity%g\n"
			"x,y,time\n",
			r,v);
	    }

/***1. Balls' loop (for each trial (given v and r)).***/
	    for (int i=0; i < ball; i++)
	    {	//Random positions inside the test region.
		double x0=(double)rand()/(double)RAND_MAX*2*a1-a1;//random x0 from (-a1,a1)
	    	double y0=(double)rand()/(double)RAND_MAX*2*a2-a2;
		double z1 = z0 + (double)rand()/(double)RAND_MAX*v*two_pi;
		
		int current_progress = i/(ball*0.1);
		if(  current_progress == progress) 
		    fprintf(stderr,"%d ", progress++);

		int mark_in = in;

/***2. String loop: search until collision.***/
		    for ( double s = 0; s < two_pi + string_unit; 
			    s+= string_unit){
			//random initial height raised within the distance v*T (T=2pi)

/***3. time loop: with booster***/		
	/*Booster: the following variables are used for accelerating the process*/
			double begin_time = (z1-z0)/v;//estimate the beginning time 
			double time_step = 0.1; //Initial time step 
			double last_dis= z1*z1 + a1*a1 + a2*a2;//use distance square for calculation.
			double t=begin_time;
			double end_time=2*z1/v;
			double new_end_time;

			//Conner booster:
			if (x0*x0+y0*y0>(a1>a2)? a2*a2: a1*a1){
			    time_step=time_unit;
			}

			while (1){
			    double height = -v*t+z1;
			    //for simplicity, use distance^2
			    double dis = (x(s,t)-x0)*(x(s,t)-x0) 
				+ (y(s,t)-y0)*(y(s,t)-y0) 
				+ (z(s,t)-height)*(z(s,t)-height);

			    if ( dis < r*r)//collision.
			    {	
				in++;
				s = two_pi + string_unit;
				if(detail){
				    //record x,y,time of the ball
				    fprintf(dp, "%g, %g, %g\n",
					    x0,y0,t);
				}
				break;//Break the while loop
			    }

			    /*Accelerating the process via zooming 
				in test region*/
			    if(dis < last_dis){
				last_dis = dis;
				begin_time = t - time_step;
				new_end_time = t + time_step;
			    }

			    t+=time_step;//move forward.

			    if ( t > end_time){

				if(time_step <= time_unit){

				    break;//Break the while loop
				}
				else { 
				    time_step *= 0.1;//try again
				    t = begin_time;
				    end_time = new_end_time;	
				}
			    }
			}
		    }
		    (mark_in == in)? out++: mark_in++;
	    }
//End of one trial with one set of velocity and rdius

	    printf("\nCounting results: %d balls intersect,"
		    " %d balls miss.\n" 
		    "The cross-section is %g. Data is stored in %s.\n"
		    ,in, out, area*in/(in+out), csv_name);

	    //fill in data
		// "Trials, time resolution, string resolution,"
	   	// "grid resolution, parameter a," 
	   	// "number of balls, velocity, radius, cross-section",
	    fprintf(fp, "\n%d,%g,%g,"
			"%d, %g," 
			"%d,%g,%g,%g",
		    count++, time_unit, string_unit,
		    ball, a0, 
		    in+out, v, r, area*in/(in+out) );
	    if(detail)   fclose(dp);
	    fclose(fp);
	}//next v trial. 
    }

/*Make a plot.*/
    if (rmin!=rmax || vmin != vmax){
	FILE *gnuplot = popen("gnuplot", "w");
	fprintf(gnuplot, "set terminal png\n"
		"set datafile separator ','\n"
		"set autoscale fix\n"
		"set key \n"
		"set ylabel 'Cross-section of the String'\n"
		"set output './%s.png'\n",
		folder);


	if (vmin != vmax){//plot cross-section vs. velocity

		fprintf(gnuplot,"set xlabel 'Velocity of the ball (unit:c)'\n"
			"set title 'Cross-section versus velocity of the balls (r = %g)'\n",
			rmax);
	    if (fit_line){
		
		fprintf(gnuplot,"f(x) = a/(x+b)\n"
			"fit f(x) '%s.csv' u 7:9 via a,b\n"
			"title_f(a,b) = sprintf('f(x) = %%.2f/(x+%%.2f)',a,b)\n"
			"plot '%s.csv' using 7:9 with points pointtype 2, f(x) t title_f(a,b)\n",
			folder, folder);
	    }
	    else{
		fprintf(gnuplot,"plot '%s.csv' using 7:9 with lines\n", folder);
	    }
	}
	else {//plot cross-section vs. radius
	    fprintf(gnuplot,"set xlabel 'Radius of the ball'\n"
		    "set title 'Cross-section versus radius of the balls (v = %g)'\n"
		    "plot '%s.csv' using 8:9 with lines\n",
		    vmax, folder);
	}
	    fprintf(gnuplot,"q\n");
    }
}

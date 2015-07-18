#include <iostream>
#include <cstdlib>
#include<mpi.h>
#include "render.hh"

int
mandelbrot(double x, double y) {
  int maxit = 511;
  double cx = x;
  double cy = y;
  double newx, newy;
  
  int it = 0;
  for (it = 0; it < maxit && (x*x + y*y) < 4; ++it) {
    newx = x*x - y*y + cx;
    newy = 2*x*y + cy;
    x = newx;
    y = newy;
  }
  return it;
}

int
main (int argc, char* argv[])
{
  int rank, size;
  int HEIGHT, WIDTH;
  if(argc == 3){
    HEIGHT = atoi(argv[1]);
    WIDTH = atoi(argv[2]);
    assert(HEIGHT > 0 && WIDTH > 0);
  }else{
    fprintf (stderr, "usage: %s <height> <width>\n", argv[0]);
    fprintf (stderr, "where <height> and <width> are the dimensions of the image.\n");
    return -1;
  }

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);


  int remainder = HEIGHT % size;
  int blocksize = HEIGHT/size;

  int noofrows = rank < remainder ? blocksize+1 : blocksize;
  int noofpixels = noofrows*WIDTH;

  double minX = -2.1;
  double maxX = 0.7;
  double minY = -1.25;
  double maxY = 1.25;

  double it = (maxY - minY)/HEIGHT;
  double jt = (maxX - minX)/WIDTH;


  float *pixels_proc = (float *)malloc(sizeof(float)*noofpixels);


  double t1=0.0,t2=0.0;
  MPI_Barrier(MPI_COMM_WORLD);   //Timer starts here
  t1=MPI_Wtime();

  //THE CYCLIC LOGIC HAS BEEN IMPLEMENTED HERE- START
  double starty = minY + rank*it;
  int k = 0;
  for(int i=0;i<noofrows;i++){
    double startx = minX;
    for(int j=0;j<WIDTH;j++){
      pixels_proc[k++] = ((float)mandelbrot(startx, starty))/512;
      startx+=jt;
    }
    starty+=(size*it);
  }
  //END
  
  MPI_Barrier(MPI_COMM_WORLD);
  t2=MPI_Wtime(); //Timer ends here

  if(rank==1)
    printf("Elapsed time:%f\n",t2-t1);

  float *ret_arr = NULL;
  int *counts = NULL;
  int *displs = NULL;
  if(rank == 0){
    ret_arr = (float *)malloc(sizeof(float)*HEIGHT*WIDTH);
    counts = (int *)malloc(sizeof(int)*size);
    displs = (int *)malloc(sizeof(int)*size);

    for(int i=0;i<size;i++){
      if(i<remainder){
        counts[i] = (blocksize+1)*WIDTH;
        displs[i] = i*(blocksize+1)*WIDTH;
      }else{
        counts[i] = blocksize*WIDTH;
        displs[i] = remainder*(blocksize+1)*WIDTH + (i-remainder)*blocksize*WIDTH;
      }
    }
  }

  MPI_Gatherv(pixels_proc, noofrows*WIDTH, MPI_FLOAT, ret_arr, counts, displs, MPI_FLOAT, 0, MPI_COMM_WORLD);

  if(rank == 0){
    gil::rgb8_image_t img(HEIGHT, WIDTH);
    auto img_view = gil::view(img);

    int k = 0;
    for(int p=0;p<size;p++){
      for(int i=p;i<HEIGHT;i+=size){
        for(int j=0;j<WIDTH;j++){
          img_view(i,j) = render(ret_arr[k]);
          k++;
        }
      }
    }
    gil::png_write_view("mandelbrot_mpi_cyclic.png",const_view(img));
  }

  MPI_Finalize();
  return 0;
}

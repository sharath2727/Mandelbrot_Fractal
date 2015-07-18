
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
    int HEIGHT, WIDTH;
    if(argc == 3){
      HEIGHT = atoi(argv[1]);
      WIDTH = atoi(argv[2]);
      assert(HEIGHT > 0 && WIDTH > 0);
    }
    else{
      fprintf (stderr, "usage: %s <height> <width>\n", argv[0]);
      fprintf (stderr, "where <height> and <width> are the dimensions of the image.\n");
      return -1;                                      
    }

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double minX = -2.1;
    double maxX = 0.7;
    double minY = -1.25;
    double maxY = 1.25;
    double it = (maxY - minY)/HEIGHT;
    double jt = (maxX - minX)/WIDTH;
    double t1=0.0;
    double t2=0.0;

    MPI_Barrier(MPI_COMM_WORLD);
    t1=MPI_Wtime();

    if(rank == 0){
      int *rowTracker = (int *)malloc(sizeof(int)*size);
      gil::rgb8_image_t img(HEIGHT, WIDTH);
      auto img_view = gil::view(img);
      int rowCounter = 0, countOfRecvRows = 0;
      for(int i=1;i<size;i++){
        rowTracker[i] = rowCounter;
        MPI_Send(&rowCounter, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        rowCounter++;
      }

    while(1 && countOfRecvRows < HEIGHT){
      float *recvdRow = (float *)malloc(sizeof(float)*WIDTH);
      MPI_Status status;
      MPI_Recv(recvdRow, WIDTH, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
      countOfRecvRows++;
      int row = rowTracker[status.MPI_SOURCE];
      for(int j=0;j<WIDTH;j++){
        img_view(row,j) = render(recvdRow[j]);
      }
      if(rowCounter < HEIGHT){
        int recvdFrom = status.MPI_SOURCE;
        rowTracker[recvdFrom] = rowCounter;
        MPI_Send(&rowCounter, 1, MPI_INT, recvdFrom, 0, MPI_COMM_WORLD);
        rowCounter++;
      }
    }

    for(int i=1;i<size;i++){
      int stopParam = -1;
      MPI_Send(&stopParam, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }   
    gil::png_write_view("mandelbrot_mpi_master_slave.png",const_view(img));
    }

    else {
      while(1){
        int recvdRowNum;
        MPI_Status status;
        MPI_Recv(&recvdRowNum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        if(recvdRowNum == -1){
          break;
        }
        float *pixVals = (float *)malloc(sizeof(float)*WIDTH);
        double startY = minY + (recvdRowNum)*it;
        double startX = minX;
        for(int x=0;x<WIDTH;x++){
          pixVals[x] = ((float)mandelbrot(startX, startY))/512;
          startX+=jt;
          }


          MPI_Send(pixVals, WIDTH, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
              }
        }

    MPI_Barrier(MPI_COMM_WORLD);
    t2=MPI_Wtime();

    if(rank==1)
      printf("Elapsed time:%f\n",t2-t1);
    MPI_Finalize();
    return 0;
}

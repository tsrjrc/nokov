/*=========================================================
 //
 // File: ClientTest.cpp
 //

 //
 =============================================================================*/

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include "cortex.h"
#include <ros/ros.h>
#include <std_msgs/Header.h>
#include <geometry_msgs/PoseStamped.h>
#include "m3x3.h"

geometry_msgs::PoseStamped pose;										

void MyErrorMsgHandler(int iLevel, const char *szMsg)
{
  const char *szLevel = NULL;

  if (iLevel == VL_Debug) {
    szLevel = "Debug";
  } else if (iLevel == VL_Info) {
    szLevel = "Info";
  } else if (iLevel == VL_Warning) {
    szLevel = "Warning";
  } else if (iLevel == VL_Error) {
    szLevel = "Error";
  }

  printf("    %s: %s\n", szLevel, szMsg);
}

 ros::Publisher local_pos_pub;

void MyDataHandler(sFrameOfData* FrameOfData)
{
	int i;
	double p0[3],// origin marker
	       p1[3],// long axis marker
	       p2[3];// plane marker
	double matrix[3][3];
	double angles[3];
	

  	printf("Received multi-cast frame no %d\n", FrameOfData->iFrame);	//current frame

  	sBodyData *Body = &FrameOfData->BodyData[0];				//BodyData[0] is the data of marker A
 	printf("x:%f y:%f z:%f\n",	Body->Markers[0][0],			//x
  					Body->Markers[0][1],			//y
  					Body->Markers[0][2]);			//z

						
 	pose.pose.position.x = Body->Markers[0][0];				
 	pose.pose.position.y = Body->Markers[0][1];
 	pose.pose.position.z = Body->Markers[0][2]; 
	//pose.pose.position.x = 1;				
 	//pose.pose.position.y = 1;
 	//pose.pose.position.z = 1; 
    	local_pos_pub.publish(pose);
	//for( i = 0; i <= 2; i++)
	//{
	//	p0[i] = Body->Markers[0][i];
	//	p1[i] = Body->Markers[1][i];
	//	p2[i] = Body->Markers[2][i];
	//}

	//M3x3_BuildVMarkerRotationMatrix(
        // p0, 
        // p1, 
        // p2, 
        // matrix);

	//M3x3_ExtractEulerAngles(
        //matrix,
        //ZYX_ORDER,
        //angles);
	//printf("1:%f 2:%f 3:%f\n",angles[0],angles[1],angles[2]);
	
	
}

int main(int argc, char* argv[])
{
  ros::init(argc, argv, "nokov_ros_node");
  ros::NodeHandle nh;
  
  local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>("/mavros/vision_pose/pose",10);						
  int retval = RC_Okay;								//state message
  unsigned char SDK_Version[4];							//version
  sBodyDefs* pBodyDefs = NULL;							

  printf("Usage: ClientTest <Me> <Cortex>\n");
  printf("	Me = My machine name or its IP address\n");
  printf("	Cortex = Cortex' machine name or its IP Address\n");

  Cortex_SetVerbosityLevel(VL_Info);						
  Cortex_GetSdkVersion(SDK_Version);
  printf("Using SDK Version %d.%d.%d\n", SDK_Version[1], SDK_Version[2],
         SDK_Version[3]);

  Cortex_SetErrorMsgHandlerFunc(MyErrorMsgHandler);
  Cortex_SetDataHandlerFunc(MyDataHandler);					//call the MyDataHandler function

  printf("****** Cortex_Initialize ******\n");
  if (argc == 1) {
    retval = Cortex_Initialize("", NULL);
  } else if (argc == 2) {
    retval = Cortex_Initialize(argv[1], NULL);
  } else if (argc == 3) {
    retval = Cortex_Initialize(argv[1], argv[2]);
  }

  if (retval != RC_Okay) {
    printf("Error: Unable to initialize ethernet communication\n");
    retval = Cortex_Exit();
    return 1;
  }

  printf("****** Cortex_GetBodyDefs ******\n");
  pBodyDefs = Cortex_GetBodyDefs();

  if (pBodyDefs == NULL) {
    printf("Failed to get body defs\n");
  } else {
    printf("Got body defs\n");
    
    printf("nMarkers:%d,nDofs:%d\n",pBodyDefs->BodyDefs[0].nMarkers,pBodyDefs->BodyDefs[0].nDofs);
    
    Cortex_FreeBodyDefs(pBodyDefs);
    pBodyDefs = NULL;
  }

  void *pResponse;
  int nBytes;
  retval = Cortex_Request("GetContextFrameRate", &pResponse, &nBytes);
  if (retval != RC_Okay)
    printf("ERROR, GetContextFrameRate\n");

  float *contextFrameRate = (float*) pResponse;

  printf("ContextFrameRate = %3.1f Hz\n", *contextFrameRate);

  printf("*** Starting live mode ***\n");
  retval = Cortex_Request("LiveMode", &pResponse, &nBytes);

  while(ros::ok())
  {
    	ros::spinOnce();
  }

  retval = Cortex_Request("Pause", &pResponse, &nBytes);
  printf("*** Paused live mode ***\n");

  printf("****** Cortex_Exit ******\n");
  retval = Cortex_Exit();

  return 0;
}

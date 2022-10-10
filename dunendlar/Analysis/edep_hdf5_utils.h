#ifndef HDF5_UTILS_H
#define HDF5_UTILS_H
//#include "hdf5.h"
#include "hdf5_hl.h"
//#include "H5Tpublic.h"



namespace hdf5_utils{

  const size_t NFIELDS_VTX = 4;
  const size_t NFIELDS_TRAJ = 14;
  const size_t NFIELDS_SEG = 25;


  //vertices
  typedef struct vertices_dt {
    unsigned int    eventID;
    float  x_vert;
    float  y_vert;
    float  z_vert;
  } vertices_dt;

  vertices_dt vertices_test;
  const char *vertices_names[NFIELDS_VTX] = {
    "eventID", "x_vert", "y_vert", "z_vert"
  };
  hid_t vertices_types[NFIELDS_VTX] = {
    H5T_STD_U32LE, H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE
  };
  size_t vertices_size = sizeof(vertices_dt);
  size_t vertices_offset[NFIELDS_VTX] = { 
    HOFFSET( vertices_dt, eventID), 
    HOFFSET( vertices_dt, x_vert), 
    HOFFSET( vertices_dt, y_vert), 
    HOFFSET( vertices_dt, z_vert)
  };
  size_t vertices_sizes[NFIELDS_VTX] = {
    sizeof(vertices_test.eventID),
    sizeof(vertices_test.x_vert),
    sizeof(vertices_test.y_vert),
    sizeof(vertices_test.z_vert)
  };

  //Trajectories
  typedef struct trajectories_dt {
    unsigned int eventID;
    unsigned int trackID; //trackID, u4
    unsigned int start_process; //start_process, u4
    unsigned int start_sub_process; //start_sub_process, u4
    unsigned int end_process; //end_process, u4
    unsigned int end_sub_process; //end_sub_process, u4
    int pdgId; //pdgId, u4
    int parentID; //parentID, i4
    float pxyz_start[3]; //pxyz_start,3, f4
    float xyz_start[3];  //xyz_start,3, f4
    float pxyz_end[3]; //pxyz_end,3, f4
    float xyz_end[3]; //xyz_end,3, f4
    float t_start; //t_start, f4
    float t_end; //t_end, f4
    float t_0; //t_0, f4
  } trajectories_dt;
  trajectories_dt trajectories_test;

  const char *trajectories_names[NFIELDS_TRAJ] = {
    "eventID", "trackID", "pdgId", "start_process", "start_sub_process",
    "end_process", "end_sub_process", "parentID", "pxyz_start", 
    "xyz_start", "pxyz_end", "xyz_end", "t_start", "t_end"
  };

  //https://stackoverflow.com/questions/15168876/hdf5-struct-with-pointer-array
  hsize_t arr_dim3[] = {3};
  hid_t vec3_type= H5Tarray_create(H5T_IEEE_F32LE, 1, arr_dim3);
  hid_t trajectories_types[NFIELDS_TRAJ] = {
    H5T_STD_U32LE,H5T_STD_U32LE,H5T_STD_U32LE,H5T_STD_U32LE,H5T_STD_U32LE,H5T_STD_U32LE,
    H5T_STD_I32LE, H5T_STD_I32LE,
    vec3_type,vec3_type,vec3_type,vec3_type,
    H5T_IEEE_F32LE,H5T_IEEE_F32LE
  };
  size_t trajectories_size = sizeof(trajectories_dt);
  size_t trajectories_offset[NFIELDS_TRAJ] = { 
    HOFFSET( trajectories_dt, eventID),
    HOFFSET( trajectories_dt, trackID),
    HOFFSET( trajectories_dt, pdgId),
    HOFFSET( trajectories_dt, start_process),
    HOFFSET( trajectories_dt, start_sub_process),
    HOFFSET( trajectories_dt, end_process),
    HOFFSET( trajectories_dt, end_sub_process),
    HOFFSET( trajectories_dt, parentID),
    HOFFSET( trajectories_dt, pxyz_start),
    HOFFSET( trajectories_dt, xyz_start),
    HOFFSET( trajectories_dt, pxyz_end),
    HOFFSET( trajectories_dt, xyz_end),
    HOFFSET( trajectories_dt, t_start),
    HOFFSET( trajectories_dt, t_end)
  };
  size_t trajectories_sizes[NFIELDS_TRAJ] = { 
    sizeof(trajectories_test.eventID),
    sizeof(trajectories_test.trackID),
    sizeof(trajectories_test.pdgId),
    sizeof(trajectories_test.start_process),
    sizeof(trajectories_test.start_sub_process),
    sizeof(trajectories_test.end_process),
    sizeof(trajectories_test.end_sub_process),
    sizeof(trajectories_test.parentID),
    sizeof(trajectories_test.pxyz_start),
    sizeof(trajectories_test.xyz_start),
    sizeof(trajectories_test.pxyz_end),
    sizeof(trajectories_test.xyz_end),
    sizeof(trajectories_test.t_start),
    sizeof(trajectories_test.t_end)
  };

  //segments
  typedef struct segments_dt {
    unsigned int eventID; //eventID; u4
    unsigned int trackID; //trackID; u4
    unsigned int n_electrons; //n_electrons; u4
    float n_photons; //n_photons; f4 ?
    int pdgId; //pdgId; i4
    int pixel_plane; //pixel_plane; i4
    float x_start; //x_start; f4
    float x_end; //x_end; f4
    float y_start; //y_start; f4
    float y_end; //y_end; f4
    float z_start; //z_start; f4
    float z_end; //z_end; f4
    float t_start; //t_start; f4
    float t_end; //t_end; f4
    float t0_start; //t0_start; f4
    float t0_end; //t0_end; f4
    float t0; //t0; f4
    float tran_diff; //tran_diff; f4
    float long_diff; //long_diff; f4
    float x; //x; f4
    float y; //y; f4
    float z; //z; f4
    float t; //t; f4
    float dE; //dE; f4
    float dEdx; //dEdx; f4
  } segments_dt;
  segments_dt segments_test;
  const char *segments_names[NFIELDS_SEG]  = { 
    "eventID", "trackID", "n_electrons", "n_photons", "pdgId", "pixel_plane",
    "x_start", "x_end", "y_start", "y_end", "z_start", "z_end", "t_start", "t_end",
    "t0_start", "t0_end", "t0", "tran_diff", "long_diff", "x", "y", "z", "t", "dE", "dEdx" 
  };
  hid_t segments_types[NFIELDS_SEG] = {
    H5T_STD_U32LE, H5T_STD_U32LE, H5T_STD_U32LE, H5T_IEEE_F32LE,
    H5T_STD_I32LE,H5T_STD_I32LE,
H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE,H5T_IEEE_F32LE
  };
  size_t segments_size = sizeof(segments_dt);
  size_t segments_offset[NFIELDS_SEG] = {
    HOFFSET( segments_dt, eventID),
    HOFFSET( segments_dt, trackID),
    HOFFSET( segments_dt, n_electrons),
    HOFFSET( segments_dt, n_photons),
    HOFFSET( segments_dt, pdgId),
    HOFFSET( segments_dt, pixel_plane),
    HOFFSET( segments_dt, x_start),
    HOFFSET( segments_dt, x_end),
    HOFFSET( segments_dt, y_start),
    HOFFSET( segments_dt, y_end),
    HOFFSET( segments_dt, z_start),
    HOFFSET( segments_dt, z_end),
    HOFFSET( segments_dt, t_start),
    HOFFSET( segments_dt, t_end),
    HOFFSET( segments_dt, t0_start),
    HOFFSET( segments_dt, t0_end),
    HOFFSET( segments_dt, t0),
    HOFFSET( segments_dt, tran_diff),
    HOFFSET( segments_dt, long_diff),
    HOFFSET( segments_dt, x),
    HOFFSET( segments_dt, y),
    HOFFSET( segments_dt, z),
    HOFFSET( segments_dt, t),
    HOFFSET( segments_dt, dE),
    HOFFSET( segments_dt, dEdx)
  };
  size_t segments_sizes[NFIELDS_SEG] = {
    sizeof(segments_test.eventID),
    sizeof(segments_test.trackID),
    sizeof(segments_test.n_electrons),
    sizeof(segments_test.n_photons),
    sizeof(segments_test.pdgId),
    sizeof(segments_test.pixel_plane),
    sizeof(segments_test.x_start),
    sizeof(segments_test.x_end),
    sizeof(segments_test.y_start),
    sizeof(segments_test.y_end),
    sizeof(segments_test.z_start),
    sizeof(segments_test.z_end),
    sizeof(segments_test.t_start),
    sizeof(segments_test.t_end),
    sizeof(segments_test.t0_start),
    sizeof(segments_test.t0_end),
    sizeof(segments_test.t0),
    sizeof(segments_test.tran_diff),
    sizeof(segments_test.long_diff),
    sizeof(segments_test.x),
    sizeof(segments_test.y),
    sizeof(segments_test.z),
    sizeof(segments_test.t),
    sizeof(segments_test.dE),
    sizeof(segments_test.dEdx)
  };

}



#endif

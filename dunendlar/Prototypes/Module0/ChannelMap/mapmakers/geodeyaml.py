#!/usr/bin/env python3

'''
  Geometry de-yamler for module0_pixel_layout_2.2.16.dat, found in
  larnd-sim/pixel_layouts.  Writes flattened text to stdout, to be
  read in by the channel map.
'''

import yaml

with open('multi_tile_layout-2.2.16.yaml') as gf:
    geo = yaml.load(gf, Loader=yaml.FullLoader)

print(geo['tile_layout_version'])

print(geo['pixel_pitch'])

print(len(geo['chip_channel_to_position']))

for chip_channel in geo['chip_channel_to_position'] :
    chip = chip_channel // 1000
    channel = chip_channel % 1000
    print(chip,channel,geo['chip_channel_to_position'][chip_channel][0],geo['chip_channel_to_position'][chip_channel][1])

icount = 0
for tile in geo['tile_chip_to_io'] :
    for chip in geo['tile_chip_to_io'][tile] :
        icount = icount + 1
print(icount)

for tile in geo['tile_chip_to_io'] :
    for chip in geo['tile_chip_to_io'][tile] :
        iogc = geo['tile_chip_to_io'][tile][chip]
        iogroup = iogc // 1000
        iochan = iogc % 1000
        print(tile,chip,iogroup,iochan)

print(len(geo['tile_indeces']))

for tile in geo['tile_indeces'] :
    print(tile,geo['tile_indeces'][tile][0],geo['tile_indeces'][tile][1])

print(len(geo['tile_orientations']))

for tile in geo['tile_orientations'] :
    print(tile,geo['tile_orientations'][tile][0],geo['tile_orientations'][tile][1],geo['tile_orientations'][tile][2])

print(len(geo['tile_positions']))

for tile in geo['tile_positions'] :
    print(tile,geo['tile_positions'][tile][0],geo['tile_positions'][tile][1],geo['tile_positions'][tile][2])

print(len(geo['tpc_centers']))

for tpc in geo['tpc_centers'] :
    print(tpc,geo['tpc_centers'][tpc][0],geo['tpc_centers'][tpc][1],geo['tpc_centers'][tpc][2])

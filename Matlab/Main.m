clc; clear all; close all;

% DataSet1 Path and Files = Composed by 3 sessions.
path = 'C:\2025\Academia\Biomedical Engineering\Matlab\Experimento1';
files = ['Dataset1_Session1.txt';'Dataset1_Session2.txt';'Dataset1_Session3.txt'];

%Dataset 1 Target
%   First Session = Target 0
%       doens't have the desired information (elementary movement) 
%   Second Session = Target 1
%       has the desired information (elementary movement)
%   Third Session: Target 0 
%       doens't have the desired information (elementary movement)
targets = [0;1;0];

dataset1 = LoadDataset(path, files, targets);

%The sensorimotor Rythm is indetified between 8 and 30 hz
%That is the L.Alpha | H.Alpha | L.Beta | H.Beta 
%The last column is the Target
waves = (6:9);

%Plot the desired brain waves 
PlotBrainWaves(dataset1,waves);

rythm  = dataset1(:,waves);
target = dataset1(:,end);

rythm = FilterMovingAvarange(rythm,16);
rythm = FilterMovingAvarange(rythm,16);

hiddenSizes = [18 12 5];
trainFcn    = 'trainbr';
performFcn  = 'crossentropy';

nerualnet = Training(hiddenSizes,trainFcn, performFcn, rythm', target');

%%

% DataSet2 Path and Files = Composed by 3 sessions.
path = 'C:\2025\Academia\Biomedical Engineering\Matlab\Experimento1';
files = ['Dataset2_Session1.txt';'Dataset2_Session2.txt'];

%Dataset 2 Target
%   First Session = Target 0
%       doens't have the desired information (elementary movement) 
%   Second Session = Target 1
%       has the desired information (elementary movement)
targets = [0;1];

dataset2 = LoadDataset(path, files, targets);

rythm  = dataset2(:,waves);
target = dataset2(:,end);

rythm = FilterMovingAvarange(rythm,16);
rythm = FilterMovingAvarange(rythm,16);

Testing(nerualnet,rythm', target');





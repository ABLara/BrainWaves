%%%%%%%%%%%%%%%%%%%%%  LoadDataset %%%%%%%%%%%%%%%%%%%%%%%%%%
% Loads the especified dataset in Experiment1, each dataset is composed by
% a especificy number of sessions.
%
%   Sessions
%       Sessions have 11 (eleven) columns. Each of them represent a
%       diferent frequency of the Raw Object, definided in Neurosky
%       Mindset Communication Protocol.
%
%   Signal order
%       %Sig.Qual | Att | Med | Delta | Theta | L.Alpha | H.Alpha | L.Beta | H.Beta | L.Gamma | M.Gamma
%
%   Parameters
%   path: Sessions Directory Name
%   file: Sessions Vector Name
%
%   Return
%       dataset = 3d Matrix with all dataset sessions
%
%   Example: 
%
%       %Target's Dataset 1 
%       First Session = Target 0
%          doens't have the desired information (elementary movement) 
%       Second Session = Target 1
%          has the desired information (elementary movement)
%       Third Session: Target 0 
%          doens't have the desired information (elementary movement)
%
%       path = 'C:\2025\Academia\Biomedical Engineering\Matlab\Experimento1';
%       files = ['Dataset1_Session1.txt';'Dataset1_Session2.txt';'Dataset1_Session3.txt'];
%       targets = [0;1;0];
%       dataset = LoadDataset(path, files, targets);
%
%%
function [dataset] = LoadDataset(path, files, targets)    
    
    for(i=1 : size(files))
        dataset(:,:,i) = load(path+"\"+files(i,:));
    end

    dataset = SetTarget(dataset, targets);
    
end
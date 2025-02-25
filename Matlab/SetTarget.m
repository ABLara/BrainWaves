%%%%%%%%%%%%%%%%%%%%%  SetTarget %%%%%%%%%%%%%%%%%%%%%%%%%%
% Based on the sessions configuration each line is targeted by 0 or 1,
% where 0 represents the dataset part that doesn't have the desired
% elementary moviment.
%
%   Parameters
%
%   dataset: LoadDataset function result
%   targets: Session classification based on session configuration
%
%   Return
%       dataset = 2D Matrix with all dataset sessions and its
%       classification (targets)

%   Example: 
%
%       
%       dataset = LoadDataset(path, files); %Dataset1
%       size(dataset) 
%       %% size result: 606 x 11 x 3
%       target = [0; 1; 0]
%       SetTarget(dataset, targets)
%
%%
function [dataset] = SetTarget(dataset3d, targets)  
    dataset =[];
    [rows,columns, pages] = size(dataset3d);

    for(i=1 : size(targets))
        expanded_targets = repelem(targets(i), rows);
        dataset2d = [dataset3d(:,:,i) expanded_targets'];
        dataset =[dataset; dataset2d];
    end 

end
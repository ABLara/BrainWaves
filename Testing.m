%%Teste NN
%
%
%hiddenSizes= [8 17];
% trainFcn='trainbr';
% performFcn='crossentropy';
% epoches=2000;
%
%


function [neuralNet] = Testing(neuralNet,dataset,target)

classification = neuralNet(dataset);
result = perform (neuralNet, target, classification);
plotconfusion(target,classification);
end 

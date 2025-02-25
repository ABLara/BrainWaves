%%Treinamento NN
%
%
%hiddenSizes= [8 17];
% trainFcn='trainbr';
% performFcn='crossentropy';
% epoches=2000;
%
%


function [neuralNet] = Training(hiddenSizes,trainFcn, performFcn, dataset, target)
    nn = patternnet(hiddenSizes,trainFcn,performFcn);
    [neuralNet,trainingRec] = train(nn,dataset,target);
    nntraintool
    plotperform(trainingRec);
end 

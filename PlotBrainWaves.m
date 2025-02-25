function [] = PlotBrainWaves(dataset, waves)
    titles = ["Sig.Qual"; "Att"; "Med"; "Delta"; "Theta"; "Low Alpha"; "High Alpha"; "Low Beta"; "High Beta"; "L.Gamma"; "M.Gamma"];
    nAmostras = 1:size(dataset);
    
    for (i=1 : numel(waves))
        figure,plot(nAmostras,dataset(:,waves(i)));
        set(gca,'FontSize',30,'FontName',"Times New Roman")
        ylim([0 50]);
        xlabel('Time (s)')
        ylabel('Amplitude')
        currentTitle = titles(waves(i));
        title(currentTitle);
        set(gcf, 'Position', [1100 1100 1100 1100]);
        saveas(gcf, '..\Imagens\'+currentTitle+'.png')
    end
end 
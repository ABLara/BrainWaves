function [dataset1] = FilterMovingAvarange(dataset,order)
    %%
    %Filtro para eliminar ruídos 
    %1°- Média móvel (filtro linear de suavizacao)
    dataset1 = [];
   [Nr Nc Np]=size(dataset);
    TF = order; 
    for(l=1:Np)
        for(j=1:Nc)
            for(i=TF:Nr)
                soma=0;
                for(k=0:TF-1)
                soma=soma+dataset(i-k,j,l);
                end
                media=soma/TF;
                dataset1(i,j,l)=media;
            end
        end
    end
end
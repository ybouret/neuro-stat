////////////////////////////////////////////////
// count coincidences with  delta lag
// X and Y are ordered arrays
////////////////////////////////////////////////

function count=count_coincidences(X,Y,delta)
    count = 0;    // number of coincidences

    //==================================
    // variables for the search
    //==================================
    Nx    = X(1); // #events in X
    Ny    = Y(1); // #events in Y
    NxMax = Nx+1; // max value for X indexing
    NyMax = Ny+1; // max value for Y indexing
    j     = 2;    // initial value for Y testing 


    //==================================
    // outer loop: on each X item
    //==================================
    for i=2:NxMax
        Xi = X(i);       // current X value
        Xlo = Xi-delta;  // current lower bound
        //------------------------------
        // find the first Y value greater
        // than the lower bound
        //------------------------------
        while( (j<=NyMax) & (Y(j)<Xlo) )
            j=j+1;
        end

        //--------------------------------
        // is it worth to test something ?
        //--------------------------------
        if(j>NyMax) then break end;

        //--------------------------------
        // test from j
        //--------------------------------
        Xup = Xi + delta; // current upper bound
        k   = j;
        while( (k<=NyMax) & (Y(k)<=Xup) )
            count = count+1;
            k     = k+1;    
        end

    end
endfunction


////////////////////////////////////////////////
// for several trials
// data1: matrix for neuron 1
// data2: matrix for neuron 2
// delta: lag time
// permut: a permutation vector
////////////////////////////////////////////////

function count=coincidence_total(data1,data2,delta,permut)
    ntrials=size(data1,1);
    count=0;
    for i=1:ntrials
        count=count+count_coincidences(data1(i,:),data2(permut(i),:),delta);
    end
endfunction

////////////////////////////////////////////////
// for several permutations, 
// gives an ordered sample of number of total coincidences
// data1: matrix for neuron 1
// data2: matrix for neuron 2
// delta: lag time
// B: integer, size of the bootstrap samples
//
// return: a sorted bootstrap sample, by increasing order
////////////////////////////////////////////////

function ech=permutation_bootstrap(data1,data2,delta,B)
    ntrials=size(data1,1);  // trials per neuron matrix
    ech=zeros(1,B);         // the bootstrap sample
    for b=1:B
        permut = grand(1,'prm',(1:ntrials)'); // random permutation
        ech(b) = coincidence_total(data1,data2,delta,permut);
    end
    //ech=gsort(ech,'g','i');    
endfunction

////////////////////////////////////////////////
// gives the empirical pvalue, 
// same values as above
////////////////////////////////////////////////

function p=permutation_pvalue(data1,data2,delta,B)
    ntrials=size(data1,1);
    
    L=[1:ntrials];  // the identity permutation
    
    True_Coinc=coincidence_total(data1,data2,delta,L); 
    ech=permutation_bootstrap(data1,data2,delta,B);

    p=sum(ech>=True_Coinc)/B;
    
endfunction


////////////////////////////////////////////////
// gives the 0-1 answer of the permutation test with level alpha
// same values as above
////////////////////////////////////////////////

function t=permutation_test(data1,data2,delta,B,alpha)
    p=permutation_pvalue(data1,data2,delta,B);
    t=sum(p<=alpha);
endfunction


////////////////////////////////////////////////
// gives an estimation of the true level of out test
////////////////////////////////////////////////

function niveau=level(a,b,lambda,nessai,delta,B,alpha,Nsimu)
    p=zeros(1:Nsimu)
    for n=1:Nsimu
        donnees=simulPP(a,b,lambda,nessai);
        p(n)=permutation_pvalue(donnees(1),donnees(2),delta,B);
        disp(n);
        disp(p(n));
    end
    niveau=sum(p<=alpha)/Nsimu;
endfunction

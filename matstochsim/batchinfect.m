sim = stochSimulation('sim_results', 1, false);
%%
A = sim.createState('A', 100);
B = sim.createComposedState('B', 0);
C = sim.createState('C', 0);
%%
A2B = sim.createPropensityReaction('A->B', 0.1);
A2B.addReactant(A);
A2B.addProduct(B);

%%
B2C = sim.createDelayReaction('B->C', B, 10);
B2C.addProduct(C);

%%
sim.run(100);
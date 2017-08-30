sim = stochSimulation('sim_results', 1, false);
A = sim.createState('A', 100);
B = sim.createState('B', 0);
A2B = sim.createReaction('A->B', 0.1);
A2B.addReactant(A);
A2B.addProduct(B);
sim.run(100);
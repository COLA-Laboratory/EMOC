#include "CDTS.h"
#include "stdafx.h"
#include "beta.h"


/*******************************************************************************
 *************************** CONSTRUCTORS **************************************
 *******************************************************************************/
CDTS::CDTS(const vector<Arm_DTS>& solutions, const vector<Arm_DTS>& subsets, double learningRate): 
arms(subsets),
full_arms(solutions),
wins(arms.size(), arms.size(), fill::zeros),
wins_full(solutions.size(), fill::zeros),
ucb(arms.size(), arms.size(), fill::ones),
lcb(arms.size(), arms.size(), fill::zeros),
theta_1(arms.size(), arms.size(), fill::zeros),
theta_2(arms.size(), arms.size(), fill::zeros),
learningRate(learningRate),
t(0),
cum_regret(0)
{
    // wins.print();
    best_choose = 0;
}

CDTS::CDTS(const mat& solutions, const mat& subsets, double learningRate):
arms(subsets.n_rows),
full_arms(solutions.n_rows),
wins(arms.size(), arms.size(), fill::zeros),
wins_full(solutions.size(), fill::zeros),
ucb(arms.size(), arms.size(), fill::ones),
lcb(arms.size(), arms.size(), fill::zeros),
theta_1(arms.size(), arms.size(), fill::zeros),
theta_2(arms.size(), fill::zeros),
learningRate(learningRate),
CopelandScore(arms.size(), fill::zeros),
t(0),
cum_regret(0)
{
    assert(subsets.n_rows == subsets.n_cols);
    assert(solutions.n_rows == solutions.n_cols);
    // initialize subsets to arms
    for(size_t i = 0; i < subsets.n_rows; i++)
    {

        vec pSubset = trans(subsets.row(i));
        // pVals.print();
        // printf("\n");
        arms[i] = Arm_DTS(pSubset);
    }
    // initialize solution winning probability
    for(size_t i = 0; i < solutions.n_rows; i++)
    {
        vec pSolutions = trans(solutions.row(i));
        full_arms[i] = Arm_DTS(pSolutions);
    }

    best_choose = 0;
    int optimal_max = 0;
    vec optimal_count(arms.size(), fill::zeros);
    for(size_t i = 0; i < wins.n_rows; i++)
    {
        // int optimal_count = 0;
        for(size_t j = 0; j < wins.n_cols; j++)
        {
            double prob = arms[i].getProbability(j);
            if(prob > 0.5)
            {
                optimal_count(i) ++;
            }
        }   
    }
    optimal_arm = optimal_count.index_max();

}

CDTS::~CDTS()
{


}


/*******************************************************************************
 ********************************* ACTIONS *************************************
 *******************************************************************************/
std::tuple<int, int, vec> CDTS::timestep(int max_round, int Trial, int subset_solution_num)
{
    std::tuple<int, int, vec> result;
    max_round = max_round;
    int best;
    while(t < max_round)
    {
        t++;
        // cout << "Round" << t << endl;

        updateUCB();

        // calculate theta_1, select the first arm
        for(size_t i = 0; i < ucb.n_rows; i++)
        {
            for(size_t j = 0; j < i; j ++)
            {
                
                random_device rd;
                mt19937 gen(rd());
                sftrabbit::beta_distribution<> beta(wins(i,j)+1, wins(j,i)+1);

                double prob = beta(gen);
                theta_1(i,j) = prob;
                theta_1(j,i) = 1.0 - theta_1(i,j);
            }
            theta_1(i,i) = 0.5;
        }
        // theta_1.print();
        CalculateCopelandScore();
        select1 = CopelandScore.index_max();
        // cout << "select 1st arm " << select1 << " max socre "<<CopelandScore(select1) << endl;

        // compute theta_2
        for(size_t i = 0; i < ucb.n_rows; i++)
        {
            random_device rd;
            mt19937 gen(rd());
            sftrabbit::beta_distribution<> beta(wins(i,select1)+1, wins(select1,i)+1);
            double prob = beta(gen);
            theta_2(i) = prob;
            
        }
        theta_2(select1) = 0.5;

        uvec rows = find(lcb.col(select1) <= 0.5);
        // cout << "theta_2" << endl;
        // theta_2.print();
        select2 = theta_2.rows(rows).index_max();
        // cout << "select 2nd arm "<< select2 << endl;
        
        // select1 = index_save[rnd];
        // cout << "c=" << select1 << endl;
        
        // cout << "d=" << select2 << endl;

        //compare arm c with d, increment w metric
        size_t full_index1 = rand() % subset_solution_num + subset_solution_num * select1;
        size_t full_index2 = rand() % subset_solution_num + subset_solution_num * select2;
        // cout << "full_index1 " << full_index1 << " full_index2 " << full_index2 << endl;
        // bool compare = arms[select1].compareWithArm(select2);
        bool compare = full_arms[full_index1].compareWithArm(full_index2);
        double regret1 = arms[optimal_arm].getProbability(select1) - 0.5;
        double regret2 = arms[optimal_arm].getProbability(select2) - 0.5;
        cum_regret += (regret1 + regret2) / 2 ;

        if(compare)
        {
            wins(select1,select2) += 1;
            wins_full(full_index1) += 1;
        }
        else
        {
            wins(select2,select1) += 1;
            wins_full(full_index2) += 1;
        }
        best = winner(Trial);
    }
    
    int ref_index = wins_full.rows(best*subset_solution_num, best*subset_solution_num + 9).index_max() + best * subset_solution_num;
    vec win_times = wins_full.rows(best*subset_solution_num, best*subset_solution_num + 9);
    // win_times.print();
    // wins.print();
    // uvec not_zero = find(wins_full > 0);
    // cout << "not_zero" <<endl;
    // not_zero.print();
    // cout << "wins_full" << endl;
    // wins_full.rows(not_zero).print();
    // cout << "preferred region plays\n" << wins_full.rows(best*subset_solution_num, best*subset_solution_num + 9) << endl;
    // cout << "most played" << wins_full.index_max() << endl;
    // cout << ref_index << endl;
    // result = std::make_pair(best, ref_index);
    result = std::make_tuple(best, ref_index, win_times);
    return result;

}

void CDTS::updateUCB()
{
    mat totalWins = wins + trans(wins);

    ucb = wins / totalWins + sqrt(learningRate * log(t) / totalWins);
    lcb = wins / totalWins - sqrt(learningRate * log(t) / totalWins);

    //Something tells me this kills all the optimization but ok...
    //Also, THERE IS PROBABLY A VECTORIZATION FOR THIS CODE
    //TODO: Find vectorization
    for(size_t i = 0; i < ucb.n_rows; i++) {
        for(size_t j = 0; j < ucb.n_cols; j++) {
            if(totalWins(i, j) == 0)
            {
                ucb(i, j) = 2;
                lcb(i,j) = 0;
            }
            if (i == j)
            {
                ucb(i,j)=0.5;
                lcb(i,j) = 0.5;
            }
                
                
        }
    }
}


void CDTS::CalculateCopelandScore()
{
    
    for(size_t i = 0; i < ucb.n_rows;i ++)
    {
        CopelandScore(i) = 0;
        for(size_t j = 0; j < ucb.n_rows; j++)
        {
            if(theta_1(i,j) > 0.5)
                CopelandScore(i) ++;
        }
    }
    // cout << "CopelanScore" << endl;
    // CopelandScore.print();
}

int CDTS::winner(int Trial)
{   int max = 0;
    size_t best = 0;
    mat totalWins = wins + trans(wins);
    win_prob = wins / totalWins;
    
    
    for(size_t i = 0; i < wins.n_rows; i++)
    {
        for(size_t j = 0; j < wins.n_cols; j++)
        {
            if(totalWins(i,j) == 0)
                win_prob(i,j) = 1;
            if(i == j)
                win_prob(i,j) = 0.5;
        }
    }
    // printf("wins:\n");
    // wins.print();
    // printf("\n winning probability:\n");
    // win_prob.print();
    for(size_t i = 0; i < win_prob.n_rows; i++ )
    {
        int count = 0;
        
        for(size_t j = 0; j < win_prob.n_cols; j++)
        {
            if(win_prob(i,j) > 0.5000)
                count ++;
        }
        if(count > max)
        {
            max = count;
            best = i;
        }
    }
    if(best == optimal_arm)
    {
        best_choose++;
    }
    double accuracy = 1.0000*best_choose / t;
    
    // printf("the optimal arm is %zu\n", optimal_arm);
    // printf("the best arm is :%zu\n",best);
    // printf("the accuracy is : %.2f % \n", accuracy*100);
    // printf("the cummulative regret is : %.4f \n", cum_regret);  

    // ofstream outputfile;
    // char filePath[256];
    // // sprintf(filePath, "./output/loss_B%d.txt",B);
    // sprintf(filePath, "./result/optimalarm_trial%d.txt", Trial);
    // outputfile.open(filePath, ios::app);
    // if(!outputfile)
    // {
    //     printf("Fail to open the file!\n");
    //     exit(1);
    // }

    // outputfile << best << endl;

    // outputfile.close();  
    return best; 
}

void CDTS::CalculateLoss(int round)
{
    double TotalLoss = 0;
    
    win_prob.print();
    for(size_t i = 0; i < win_prob.n_rows; i ++)
    {
        for(size_t j = i; j < win_prob.n_cols; j++)
        {
            // double true_proba = arms[i].getProbability(j);
            TotalLoss += pow(win_prob(i,j) - arms[i].getProbability(j), 2);
            TotalLoss += pow(win_prob(j,i) - arms[j].getProbability(i), 2);

        }
    }
    // // TotalLoss /= arms.size();
    // cout << "Total Loss is :" << TotalLoss << endl;
    printf("Total Loss: %f\n", TotalLoss);
    // ofstream outputfile;
    // char filePath[256];
    // sprintf(filePath, "./output/loss_Round%d.txt",round);
    // outputfile.open(filePath, ios::app);
    // if(!outputfile)
    // {
    //     printf("Fail to open the file!\n");
    //     exit(1);
    // }

    // outputfile << TotalLoss << endl;

    // outputfile.close();
    
    
}

ostream& operator<<(ostream& os, const CDTS& r)
{
    os << "Run arms: " << endl;
    for(size_t i = 0; i < r.arms.size(); i++) {
        os << "\t[" << i << "]: " << r.arms[i] << endl;
    }

    os << endl << "UCB: " << endl;
    os << r.ucb << endl;

    return os;
}
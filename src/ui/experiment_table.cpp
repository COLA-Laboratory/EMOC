#include "ui/experiment_table.h"

#include <cmath>
#include <map>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "imgui.h"
#include "core/macro.h"
#include "core/file.h"
#include "core/emoc_manager.h"
#include "ui/ui_utility.h"
#include "ui/uipanel_manager.h"

namespace emoc{

	static double CalculateDisplayIndicator(const std::vector<double>& indicator_history, const std::vector<bool>& is_indicator_record, bool& is_valid)
	{
		double res = 0.0;
		int count = 0;
		for (int i = 0; i < indicator_history.size(); i++)
		{
			if (is_indicator_record[i])
			{
				res += indicator_history[i];
				count++;
			}
		}
		if (count > 0) is_valid = true;
		return res / (double)count;
	}

	ExperimentTable::ExperimentTable()
	{
		// do nothing...
	}

	ExperimentTable::~ExperimentTable()
	{

	}

	void ExperimentTable::Render(bool is_displayM, bool is_displayD, bool is_displayN, bool is_displayEvaluation, 
		const std::string &display_para, const std::string& format, const std::string& hypothesis, const std::string& comp)
	{
		// update current table state
		isM = is_displayM; isD = is_displayD; isN = is_displayN; isEvaluation = is_displayEvaluation;
		para_name = display_para; format_name = format; hypothesis_name = hypothesis; comp_name = comp;

		// set ImGui table flag
		static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
		const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
		const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

		std::vector<std::string> columns;
		// set this frame's extra columns
		if (is_displayM) columns.push_back("M");
		if (is_displayD) columns.push_back("D");
		if (is_displayN) columns.push_back("N");
		if (is_displayEvaluation) columns.push_back("Evaluation");

		// When using ScrollX or ScrollY we need to specify a size for our table container!
		// Otherwise by default the table will fit all available space, like a BeginChild() call.
		ImVec2 outer_size = ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 5.0f);
		int column_count = algorithm_names.size() + columns.size() + 1;
		if (ImGui::BeginTable("ResultTable##Exp", column_count, flags, outer_size))
		{
			ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible

			// Instead of calling TableHeadersRow() we'll submit custom headers ourselves
			ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

			// first column is an empty header
			ImGui::TableHeader("    ");
			for (int c = 0; c < columns.size(); c++)
			{
				ImGui::TableSetColumnIndex(c + 1);
				const char* column_name = columns[c].c_str();
				ImGui::TableHeader(column_name);
			}
			for (int c = 0; c < algorithm_names.size(); c++)
			{
				ImGui::TableSetColumnIndex(c + columns.size() + 1);
				const char* column_name = algorithm_names[c].c_str();
				ImGui::TableHeader(column_name);
			}

			int row_num = rows.size();
			for (int row = 0; row < row_num; row++)
			{
				float current_row_height = rows[row].Ds.size() * TEXT_BASE_HEIGHT;
				ImGui::TableNextRow(ImGuiTableRowFlags_None, current_row_height);

				for (int c = 0; c < column_count; c++)
				{
					ImGui::TableSetColumnIndex(c);
					if (c == 0)
					{
						ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.18f, 0.18f, 0.32f, 1.00f));
						ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
						TextCenterInTableCell(rows[row].problem_name, current_row_height);
					}
					else if (c > 0 && c < columns.size() + 1)
					{
						int col_index = c - 1;
						DisplayTableProblemProperty(columns[col_index], row, rows[row].Ds.size());
					}
					else
					{
						// update table's data when data is ready
						if (EMOCManager::Instance()->GetMultiThreadDataState())
						{
							int col_in_algorithms = c - columns.size() - 1; // the -1 is counting for the first empty column header
							DisplayTableResult(row, rows[row].Ds.size(), col_in_algorithms, display_para, format, hypothesis);
						}
					}
				}
			}

			ImGui::EndTable();
		}

	}

	bool ExperimentTable::Save()
	{
		CreateDirectTable();
		bool res1 = SaveToCSV();
		bool res2 = SaveToLatex();

		return res1 && res2;
	}

	void ExperimentTable::UpdateExperimentTable(const std::vector<std::string>& algorithms, const std::vector<std::string>& problems, const std::vector<int>& Ms,
		const std::vector<int>& Ds, const std::vector<int>& Ns, const std::vector<int>& Evaluations, const std::vector<int>& parameter_indexes)
	{
		// clear existed datas
		algorithm_names.clear();
		problem_map.clear();
		rows.clear();

		// initial experiment table datas
		algorithm_names = algorithms;
		for (int i = 0; i < problems.size(); i++)
		{
			ProblemTableProperty p;
			p.M = Ms[i];
			p.D = Ds[i];
			p.N = Ns[i];
			p.Evaluation = Evaluations[i];
			p.parameter_index = parameter_indexes[i];

			problem_map[problems[i]].push_back(p);
		}

		// format the table structure
		std::unordered_map<std::string, bool> selected_map;
		for (int i = 0; i < problems.size(); i++)
		{
			std::string problem_name = problems[i];
			if (selected_map.find(problem_name) != selected_map.end())
				continue;

			selected_map[problem_name] = true;
			const std::vector<ProblemTableProperty>& p = problem_map[problem_name];

			// We reuse the ProblemTableProperty struct here, for storing the D, Evaluation and parameter_index.
			std::map<int, std::vector<ProblemTableProperty>> M_map_Others;
			for (int j = 0; j < p.size(); j++)
			{
				M_map_Others[p[j].M].push_back(p[j]);
			}

			// Different M with same problem can be different rows, each row can be further divided according to Ds.
			for (auto iter2 = M_map_Others.begin(); iter2 != M_map_Others.end(); iter2++)
			{
				int current_M = iter2->first;
				std::vector<ProblemTableProperty> temp = iter2->second;

				std::sort(temp.begin(), temp.end(), [=](ProblemTableProperty& left, ProblemTableProperty& right) {
					return left.D < right.D;
					});

				TableRowProperty t;
				t.problem_name = problem_name;
				t.M = current_M;
				for (int k = 0; k < temp.size(); k++)
				{
					t.Ds.push_back(temp[k].D);
					t.Ns.push_back(temp[k].N);
					t.Evaluations.push_back(temp[k].Evaluation);
					t.parameter_indexes.push_back(temp[k].parameter_index);
				}

				rows.push_back(t);
			}
			
		}
		

	}

	void ExperimentTable::PrintTable()
	{
		std::cout << "\t|M\t|D\t|";
		for (int j = 0; j < algorithm_names.size(); j++)
		{
			std::cout << algorithm_names[j] << "\t|";
		}
		std::cout << "\n-----------------------------------------------------------------------------------\n";
		for (int i = 0; i < rows.size(); i++)
		{
			int current_height = rows[i].Ds.size();

			for (int j = 0; j < current_height; j++)
			{
				// print problem name and M
				if (j == (current_height - 1) / 2)
				{
					std::cout << rows[i].problem_name << "\t|";
					std::cout << rows[i].M << "\t|";
				}
				else
				{
					std::cout << "\t|";
					std::cout << "\t|";
				}
				// print D
				std::cout << rows[i].Ds[j] << "\t|";

				// for algorithms metric
				for (int k = 0; k < algorithm_names.size(); k++)
				{
					std::cout << "\t\t|";
				}

				if (j == current_height - 1)
					std::cout << "\n------------------------------------------------------------------------------------\n";
				else
					std::cout << "\n\t\t-------------------------------------------------------------------\n";
			}
		}
	}

	void ExperimentTable::PrintMetric(std::string metric_name)
	{
		for (int i = 0; i < rows.size(); i++)
		{
			int current_height = rows[i].Ds.size();

			for (int j = 0; j < current_height; j++)
			{
				// for algorithms metric
				for (int k = 0; k < algorithm_names.size(); k++)
				{
					int parameter_index = rows[i].parameter_indexes[j] + k;
					EMOCMultiThreadResult res = EMOCManager::Instance()->GetMultiThreadResult(parameter_index);


					double mean = 0.0;
					double std = 0.0;
					int count = 0;
					std::vector<double> indicator_history;
					std::vector<bool> indicator_record;
					if (metric_name == "IGD")
					{
						indicator_history = res.igd.metric_history;
						indicator_record = res.igd.is_record;
					}
					else if (metric_name == "HV")
					{
						indicator_history = res.hv.metric_history;
						indicator_record = res.hv.is_record;
					}

					for (int c = 0; c < indicator_history.size(); c++)
					{
						if (indicator_record[c])
						{
							mean += indicator_history[c];
							count++;
						}
					}
					mean = mean / count;
					for (int c = 0; c < indicator_history.size(); c++)
					{
						if (indicator_record[c])
							std += (indicator_history[c] - mean) * (indicator_history[c] - mean);
					}
					std = std / count; std = std::sqrt(std);

					printf("%.2e(%.2e)\t", mean, std );
				}
				std::cout << "\n";
			}
		}
	}


	void ExperimentTable::DisplayTableProblemProperty(const std::string& col_name, int row, int row_height)
	{
		if (col_name == "M")
		{
			TextCenterInTableCell(std::to_string(rows[row].M), row_height * ImGui::GetTextLineHeightWithSpacing());
		}
		else
		{
			for (int i = 0; i < row_height; i++)
			{
				if (col_name == "D")
					ImGui::Text("%d", rows[row].Ds[i]);
				else if (col_name == "N")
					ImGui::Text("%d", rows[row].Ns[i]);
				else if (col_name == "Evaluation")
					ImGui::Text("%d", rows[row].Evaluations[i]);

				if (i == row_height - 1) break;
				ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.33f, 0.33f, 0.35f, 1.00f));
				ImGui::Separator();
				ImGui::PopStyleColor();
			}
		}

	}

	void ExperimentTable::DisplayTableResult(int row, int row_height, int col_in_algorithms, const std::string& para, const std::string& format, const std::string &hypothesis)
	{
		for(int i = 0;i < row_height;i++)
		{ 
			int parameter_index = rows[row].parameter_indexes[i] + col_in_algorithms;
			EMOCMultiThreadResult res = EMOCManager::Instance()->GetMultiThreadResult(parameter_index);
			double mean = 0.0, std = 0.0, median = 0.0, iqr = 0.0;
			char hypothesis_symbol = ' ';

			// calculate correct comparision result index
			int comp_index = 2;
			if (comp_name == "Default") comp_index = 2;
			else if (comp_name == "Best" && (format == "Median" || format == "Median(IQR)")) comp_index = 1;
			else if (comp_name == "Best" && (format == "Mean" || format == "Mean(STD)")) comp_index = 0;

			// set display table content according to the metric and result stored in EMOCMultiThreadResult
			char display[256];
			char final_display[256];
			if (para == "Runtime")
			{
				hypothesis_symbol = GetHypothesisSymbol(res.runtime.metric_mean_ranksum[comp_index], res.runtime.metric_median_ranksum[comp_index], res.runtime.metric_mean_signrank[comp_index], res.runtime.metric_median_signrank[comp_index], hypothesis, format);
				SetTableContent(display, format, res.runtime.metric_mean, res.runtime.metric_std, res.runtime.metric_median, res.runtime.metric_iqr);
			}
			else if (para == "IGD")
			{
				hypothesis_symbol = GetHypothesisSymbol(res.igd.metric_mean_ranksum[comp_index], res.igd.metric_median_ranksum[comp_index], res.igd.metric_mean_signrank[comp_index], res.igd.metric_median_signrank[comp_index], hypothesis, format);
				SetTableContent(display, format, res.igd.metric_mean, res.igd.metric_std, res.igd.metric_median, res.igd.metric_iqr);
				//std::cout << hypothesis_symbol << "\n";
			}
			else if (para == "HV")
			{
				hypothesis_symbol = GetHypothesisSymbol(res.hv.metric_mean_ranksum[comp_index], res.hv.metric_median_ranksum[comp_index], res.hv.metric_mean_signrank[comp_index], res.hv.metric_median_signrank[comp_index], hypothesis, format);
				SetTableContent(display, format, res.hv.metric_mean, res.hv.metric_std, res.hv.metric_median, res.hv.metric_iqr);
			}
			else if (para == "GD")
			{
				hypothesis_symbol = GetHypothesisSymbol(res.gd.metric_mean_ranksum[comp_index], res.gd.metric_median_ranksum[comp_index], res.gd.metric_mean_signrank[comp_index], res.gd.metric_median_signrank[comp_index], hypothesis, format);
				SetTableContent(display, format, res.gd.metric_mean, res.gd.metric_std, res.gd.metric_median, res.gd.metric_iqr);
			}
			else if (para == "Spacing")
			{
				hypothesis_symbol = GetHypothesisSymbol(res.spacing.metric_mean_ranksum[comp_index], res.spacing.metric_median_ranksum[comp_index], res.spacing.metric_mean_signrank[comp_index], res.spacing.metric_median_signrank[comp_index], hypothesis, format);
				SetTableContent(display, format, res.spacing.metric_mean, res.spacing.metric_std, res.spacing.metric_median, res.spacing.metric_iqr);
			}
			else if (para == "IGDPlus")
			{
				hypothesis_symbol = GetHypothesisSymbol(res.igdplus.metric_mean_ranksum[comp_index], res.igdplus.metric_median_ranksum[comp_index], res.igdplus.metric_mean_signrank[comp_index], res.igdplus.metric_median_signrank[comp_index], hypothesis, format);
				SetTableContent(display, format, res.igdplus.metric_mean, res.igdplus.metric_std, res.igdplus.metric_median, res.igdplus.metric_iqr);
			}
			else if (para == "GDPlus")
			{
				hypothesis_symbol = GetHypothesisSymbol(res.gdplus.metric_mean_ranksum[comp_index], res.gdplus.metric_median_ranksum[comp_index], res.gdplus.metric_mean_signrank[comp_index], res.gdplus.metric_median_signrank[comp_index], hypothesis, format);
				SetTableContent(display, format, res.gdplus.metric_mean, res.gdplus.metric_std, res.gdplus.metric_median, res.gdplus.metric_iqr);
			}
			else if (para == "BestValue")
			{
				hypothesis_symbol = GetHypothesisSymbol(res.best_value.metric_mean_ranksum[comp_index], res.best_value.metric_median_ranksum[comp_index], res.best_value.metric_mean_signrank[comp_index], res.best_value.metric_median_signrank[comp_index], hypothesis, format);
				SetTableContent(display, format, res.best_value.metric_mean, res.best_value.metric_std, res.best_value.metric_median, res.best_value.metric_iqr);
			}
			else
			{
				// TODO: ADD MORE METRICS
			}

			// The comparision is simple, so we put the logic in ui rendering loop. 
			bool is_best = CheckIsBest(para, format, parameter_index); 

			if (is_best) ImGui::PushFont(UIPanelManager::Instance()->font_bold);
			if (res.valid_res_count > 0)
			{
				sprintf(final_display, "%s%c", display, hypothesis_symbol);
				ImGui::Text(final_display);
			}
			else
				ImGui::Text(" ");
			if (is_best) ImGui::PopFont();

			if (i == row_height - 1) break;
			ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.33f, 0.33f, 0.35f, 1.00f));
			ImGui::Separator();
			ImGui::PopStyleColor();
		}
		
	}

	void ExperimentTable::SetTableContent(char* display, const std::string& format, double mean, double std, double median, double iqr)
	{
		if (format == "Mean")
			sprintf(display, "%.2e", mean);
		else if (format == "Mean(STD)")
			sprintf(display, "%.2e(%.2e)", mean, std);
		else if (format == "Median")
			sprintf(display, "%.2e", median);
		else if (format == "Median(IQR)")
			sprintf(display, "%.2e(%.2e)", median, iqr );
		else
			sprintf(display, "  ");
	}

	bool ExperimentTable::CheckIsBest(const std::string& para, const std::string& format, int parameter_index)
	{
		int algorithm_num = algorithm_names.size();
		int range_index = parameter_index / algorithm_num;
		int range_start = range_index * algorithm_num, range_end = (range_index + 1) * algorithm_num;

		int best_index = range_start;
		double best_metric1 = 0.0, best_metric2 = 0.0;
		GetComparedMetric(para, format, range_start, best_metric1, best_metric2);
		bool is_min_better = true;
		if (para == "HV") is_min_better = false;

		for (int i = range_start + 1; i < range_end; i++)
		{
			double metric1 = 0.0, metric2 = 0.0;
			GetComparedMetric(para, format, i, metric1, metric2);

			if ((is_min_better && best_metric1 > metric1) || (!is_min_better && best_metric1 < metric1))
			{
				best_index = i;
				best_metric1 = metric1;
			}
			else if (std::fabs(best_metric1 - metric1) <= EMOC_EPS)
			{
				if (format == "Median" || format == "Median(IQR)")
				{
					if ((is_min_better && best_metric2 > metric2) || (!is_min_better && best_metric2 < metric2))
					{
						best_index = i;
						best_metric2 = metric2;
					}
				}
				else if (format == "Mean" || format == "Mean(STD)")	 // standard deviation is still the smaller the better
				{
					if (best_metric2 > metric2)
					{
						best_index = i;
						best_metric2 = metric2;
					}
				}
			}
		}

		return best_index == parameter_index;
	}

	char ExperimentTable::GetHypothesisSymbol(int mean_ranksum, int median_ranksum, int mean_signrank, int median_signrank,
		const std::string& hypothesis, const std::string& format)
	{
		char symbol[3] = { '-', '=', '+' };
		char res = ' ';

		if (format == "Mean" || format == "Mean(STD)")
		{
			if (hypothesis == "RankSumTest")
				if (mean_ranksum != -2) res = symbol[mean_ranksum + 1];

			if (hypothesis == "SignRankTest")
				if (mean_signrank != -2)  res = symbol[mean_signrank + 1];
			
		}
		else if (format == "Median" || format == "Median(IQR)")
		{
			if (hypothesis == "RankSumTest")
				if (median_ranksum != -2) res = symbol[median_ranksum + 1];

			if (hypothesis == "SignRankTest")
				if (median_signrank != -2)  res = symbol[median_signrank + 1];
		}

		return res;
	}

	bool ExperimentTable::CreateDirectTable()
	{
		bool res = true;

		// set extra columns according to current table's state
		std::vector<std::string> columns;
		if (isM) columns.push_back("M");
		if (isD) columns.push_back("D");
		if (isN) columns.push_back("N");
		if (isEvaluation) columns.push_back("Evaluation");

		// initialize direct table
		int column_num = algorithm_names.size() + columns.size() + 1;
		direct_table.clear();

		// first row
		direct_table.push_back(std::vector<std::string>(column_num, ""));
		direct_table[0][0] = " ";
		for (int i = 0; i < columns.size(); i++)
			direct_table[0][i + 1] = columns[i];
		
		for (int i = 0; i < algorithm_names.size(); i++)
			direct_table[0][i + columns.size() + 1] = algorithm_names[i];

		// other rows
		int current_row = 1;
		for (int r = 0; r < rows.size(); r++)
		{
			int t = rows[r].Ds.size();
			for(int j = 0;j < t;j++)
				direct_table.push_back(std::vector<std::string>(column_num, ""));


			for (int c = 0; c < column_num; c++)
			{
				if (c == 0)
				{
					for (int j = 0; j < t; j++)
						direct_table[current_row + j][c] = rows[r].problem_name;
				}
				else if (c > 0 && c < columns.size() + 1)
				{
					for (int j = 0; j < t; j++)
					{
						int col_index = c - 1;
						std::string col_name = columns[c - 1];

						if (col_name == "M")
							direct_table[current_row + j][c] = std::to_string(rows[r].M);
						else if (col_name == "D")
							direct_table[current_row + j][c] = std::to_string(rows[r].Ds[j]);
						else if (col_name == "N")
							direct_table[current_row + j][c] = std::to_string(rows[r].Ns[j]);
						else if (col_name == "Evaluation")
							direct_table[current_row + j][c] = std::to_string(rows[r].Evaluations[j]);
					}
				}
				else
				{
					for (int j = 0; j < t; j++)
					{
						int col_in_algorithms = c - columns.size() - 1;

						int parameter_index = rows[r].parameter_indexes[j] + col_in_algorithms;
						EMOCMultiThreadResult res = EMOCManager::Instance()->GetMultiThreadResult(parameter_index);

						double mean = 0.0, std = 0.0, median = 0.0, iqr = 0.0;
						char hypothesis_symbol = ' ';

						// calculate correct comparision result index
						int comp_index = 2;
						if (comp_name == "Default") comp_index = 2;
						else if (comp_name == "Best" && (format_name == "Median" || format_name == "Median(IQR)")) comp_index = 1;
						else if (comp_name == "Best" && (format_name == "Mean" || format_name == "Mean(Std)")) comp_index = 0;

						// set display table content according to the metric and result stored in EMOCMultiThreadResult
						char display[256];
						char final_display[256];
						if (para_name == "Runtime")
						{
							hypothesis_symbol = GetHypothesisSymbol(res.runtime.metric_mean_ranksum[comp_index], res.runtime.metric_median_ranksum[comp_index], res.runtime.metric_mean_signrank[comp_index], res.runtime.metric_median_signrank[comp_index], hypothesis_name, format_name);
							SetTableContent(display, format_name, res.runtime.metric_mean, res.runtime.metric_std, res.runtime.metric_median, res.runtime.metric_iqr);
						}
						else if (para_name == "IGD")
						{
							hypothesis_symbol = GetHypothesisSymbol(res.igd.metric_mean_ranksum[comp_index], res.igd.metric_median_ranksum[comp_index], res.igd.metric_mean_signrank[comp_index], res.igd.metric_median_signrank[comp_index], hypothesis_name, format_name);
							SetTableContent(display, format_name, res.igd.metric_mean, res.igd.metric_std, res.igd.metric_median, res.igd.metric_iqr);
						}
						else if (para_name == "HV")
						{
							hypothesis_symbol = GetHypothesisSymbol(res.hv.metric_mean_ranksum[comp_index], res.hv.metric_median_ranksum[comp_index], res.hv.metric_mean_signrank[comp_index], res.hv.metric_median_signrank[comp_index], hypothesis_name, format_name);
							SetTableContent(display, format_name, res.hv.metric_mean, res.hv.metric_std, res.hv.metric_median, res.hv.metric_iqr);
						}
						else if (para_name == "GD")
						{
							hypothesis_symbol = GetHypothesisSymbol(res.gd.metric_mean_ranksum[comp_index], res.gd.metric_median_ranksum[comp_index], res.gd.metric_mean_signrank[comp_index], res.gd.metric_median_signrank[comp_index], hypothesis_name, format_name);
							SetTableContent(display, format_name, res.gd.metric_mean, res.gd.metric_std, res.gd.metric_median, res.gd.metric_iqr);
						}
						else if (para_name == "Spacing")
						{
							hypothesis_symbol = GetHypothesisSymbol(res.spacing.metric_mean_ranksum[comp_index], res.spacing.metric_median_ranksum[comp_index], res.spacing.metric_mean_signrank[comp_index], res.spacing.metric_median_signrank[comp_index], hypothesis_name, format_name);
							SetTableContent(display, format_name, res.spacing.metric_mean, res.spacing.metric_std, res.spacing.metric_median, res.spacing.metric_iqr);
						}
						else if (para_name == "IGDPlus")
						{
							hypothesis_symbol = GetHypothesisSymbol(res.igdplus.metric_mean_ranksum[comp_index], res.igdplus.metric_median_ranksum[comp_index], res.igdplus.metric_mean_signrank[comp_index], res.igdplus.metric_median_signrank[comp_index], hypothesis_name, format_name);
							SetTableContent(display, format_name, res.igdplus.metric_mean, res.igdplus.metric_std, res.igdplus.metric_median, res.igdplus.metric_iqr);
						}
						else if (para_name == "GDPlus")
						{
							hypothesis_symbol = GetHypothesisSymbol(res.gdplus.metric_mean_ranksum[comp_index], res.gdplus.metric_median_ranksum[comp_index], res.gdplus.metric_mean_signrank[comp_index], res.gdplus.metric_median_signrank[comp_index], hypothesis_name, format_name);
							SetTableContent(display, format_name, res.gdplus.metric_mean, res.gdplus.metric_std, res.gdplus.metric_median, res.gdplus.metric_iqr);
						}
						else if (para_name == "BestValue")
						{
							hypothesis_symbol = GetHypothesisSymbol(res.best_value.metric_mean_ranksum[comp_index], res.best_value.metric_median_ranksum[comp_index], res.best_value.metric_mean_signrank[comp_index], res.best_value.metric_median_signrank[comp_index], hypothesis_name, format_name);
							SetTableContent(display, format_name, res.best_value.metric_mean, res.best_value.metric_std, res.best_value.metric_median, res.best_value.metric_iqr);
						}
						else
						{
							// TODO: ADD MORE METRICS
						}

						// The comparision is simple, so we put the logic in ui rendering loop. 
						bool is_best = CheckIsBest(para_name, format_name, parameter_index);
						sprintf(final_display, "%s%c", display, hypothesis_symbol);
						if (is_best) sprintf(final_display, "b%s%c", display,hypothesis_symbol);

						direct_table[current_row + j][c] = final_display;
					}
				}
			}

			current_row += t;
		}

		return res;
	}

	bool ExperimentTable::SaveToCSV()
	{
		bool res = true;
		char dir[256];
		char filepath[256];
		sprintf(dir, "./output/table/");
		sprintf(filepath, "%snew.csv", dir);
		CreateDirectory(dir);
		std::ofstream  data_file(filepath);
		if (!data_file)
		{
			std::cerr << "Can not open" << filepath << "!\n";
			return false;
		}
		else
		{
			for (int i = 0; i < direct_table.size(); i++)
			{
				for (int j = 0; j < direct_table[0].size(); j++)
				{
					if (direct_table[i][j][0] == 'b')
						data_file << direct_table[i][j].substr(1)<<",";
					else
						data_file << direct_table[i][j] << ",";

				}
				data_file << "\n";
			}
		}

		data_file.close();
		return res;
	}

	bool ExperimentTable::SaveToLatex()
	{
		bool res = true;
		char dir[256];
		char filepath[256];
		sprintf(dir, "./output/table/");
		sprintf(filepath, "%snew.tex", dir);
		CreateDirectory(dir);

		std::ofstream  data_file(filepath);
		if (!data_file)
		{
			std::cerr << "Can not open" << filepath << "!\n";
			return false;
		}
		else
		{
			int row_num = direct_table.size();
			int col_num = direct_table[0].size();

			int extra_col = 0;
			if (isM) extra_col++;
			if (isD) extra_col++;
			if (isN) extra_col++;
			if (isEvaluation) extra_col++;

			// create latex code
			data_file << "\\documentclass[journal]{IEEEtran}\n";
			data_file << "\\usepackage[table]{xcolor}\n";
			data_file << "\\usepackage{multirow,hhline}\n";
			data_file << "\\begin{document}\n";
			data_file << "\\begin{table*}[]\n";
			data_file << "\\centering\n";
			data_file << "\\caption{EMOC Result Table}\n";
			data_file << "\\begin{tabular}{";

			for (int i = 0; i < col_num - 1; i++) data_file << "c|";
			data_file << "c}\n";
			data_file << "\\hline\n";

			// first header row
			for (int j = 0; j < col_num - 1; j++)
				data_file << direct_table[0][j] << "  &";
			data_file << direct_table[0][col_num - 1] << "\\" << "\\" << "\\hline\\hline\n";

			// following rows
			char problem[256];
			char data[256];
			int current_row = 1;
			for (int r = 0; r < rows.size(); r++)
			{
				int multi_row = rows[r].Ds.size();

				for (int j = 0; j < multi_row; j++)
				{
					// problem names
					if (multi_row > 1)
					{
						sprintf(problem, "\\multirow{%d}{*}{%s}", multi_row, direct_table[current_row + j][0].c_str());
					}
					else sprintf(problem, "%s", direct_table[current_row + j][0].c_str());
					if (j == 0) data_file << problem;
					data_file << "  &";

					// count for extra columns, e.g. M,D,N,Evaluation
					for (int c = 1; c < extra_col + 1; c++)
					{
						if (direct_table[0][c] == "M")
						{
							if (multi_row > 1) sprintf(data, "\\multirow{%d}{*}{%s}", multi_row, direct_table[current_row + j][c].c_str());
							else sprintf(data, "%s", direct_table[current_row + j][c].c_str());
							if (j == 0) data_file << data;
						}
						else
						{
							sprintf(data, "%s", direct_table[current_row + j][c].c_str());
							data_file << data;
						}
						data_file << "  &";
					}

					// metric data
					for (int c = extra_col + 1; c < col_num; c++)
					{
						// if is best
						if(direct_table[current_row+j][c][0] == 'b')
							sprintf(data, "\\cellcolor[gray]{0.8}\\textbf{%s} ", direct_table[current_row + j][c].substr(1).c_str());
						else
							sprintf(data, "%s", direct_table[current_row + j][c].c_str());

						data_file << data;
						if(c != col_num - 1) data_file << "  &";
					}

					// table format things
					data_file << "\\" << "\\";
					if (multi_row > 1 && j != multi_row - 1)
					{
						data_file << "\\hhline{~|";
						if (isM)
						{
							data_file << "~|";
							for (int k = 0; k < col_num - 3; k++) data_file << "-|";
							data_file << "-}\n";
						}
						else
						{
							for (int k = 0; k < col_num - 2; k++) data_file << "-|";
							data_file << "-}\n";
						}
					}
					else
						data_file << "\\hline\n";
				}
				current_row += multi_row;
			}

			data_file << "\\end{tabular}\n";
			data_file << "\\end{table*}\n";
			data_file << "\\end{document}";
		}

		data_file.close();
		return res;
	}

}
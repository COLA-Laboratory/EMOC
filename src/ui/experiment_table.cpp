#include "ui/experiment_table.h"

#include <map>
#include <algorithm>
#include <iostream>

#include "core/emoc_manager.h"
#include "imgui.h"
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

	// ImVec4(0.33f, 0.33f, 0.35f, 1.00f);
	void ExperimentTable::Render(bool is_displayM, bool is_displayD, bool is_displayN, bool is_displayEvaluation, 
		const std::string &display_para, const std::string& format)
	{
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
							DisplayTableResult(row, rows[row].Ds.size(), col_in_algorithms, display_para, format);
						}
					}
				}
			}

			ImGui::EndTable();
		}

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
						indicator_history = res.igd_history;
						indicator_record = res.is_igd_record;
					}
					else if (metric_name == "HV")
					{
						indicator_history = res.hv_history;
						indicator_record = res.is_hv_record;
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

	void ExperimentTable::DisplayTableResult(int row, int row_height, int col_in_algorithms, const std::string& para, const std::string& format)
	{
		for(int i = 0;i < row_height;i++)
		{ 
			int parameter_index = rows[row].parameter_indexes[i] + col_in_algorithms;
			EMOCMultiThreadResult res = EMOCManager::Instance()->GetMultiThreadResult(parameter_index);
			double mean = 0.0, std = 0.0, median = 0.0, iqr = 0.0;

			// set display table content according to the metric and result stored in EMOCMultiThreadResult
			char display[256];
			if (para == "Runtime")
				SetTableContent(display, format, res.runtime_mean, res.runtime_std, res.runtime_median, res.runtime_iqr);
			else if (para == "IGD")
				SetTableContent(display, format, res.igd_mean, res.igd_std, res.igd_median, res.igd_iqr);
			else if (para == "HV")
				SetTableContent(display, format, res.hv_mean, res.hv_std, res.hv_median, res.hv_iqr);

			// The comparision is simple, so we put the logic in ui rendering loop. 
			bool is_best = CheckIsBest(para, format, parameter_index); 

			if (is_best) ImGui::PushFont(UIPanelManager::Instance()->font_bold);
			if (res.valid_res_count > 0)
				ImGui::Text(display);
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
		for (int i = range_start + 1; i < range_end; i++)
		{
			double metric1 = 0.0, metric2 = 0.0;
			GetComparedMetric(para, format, i, metric1, metric2);
			if (best_metric1 > metric1)
			{
				best_index = i;
				best_metric1 = metric1;
			}
			else if (std::fabs(best_metric1 - metric1) <= EMOC_EPS)
			{
				if (best_metric2 > metric2)
				{
					best_index = i;
					best_metric2 = metric2;
				}
			}
		}

		return best_index == parameter_index;
	}

	void ExperimentTable::GetComparedMetric(const std::string& para, const std::string& format, int parameter_index, double& metric1, double& metric2)
	{
		EMOCMultiThreadResult res = EMOCManager::Instance()->GetMultiThreadResult(parameter_index);
		if (res.valid_res_count == 0)
		{
			metric1 = EMOC_INF;
			metric2 = EMOC_INF;
			return;
		}

		if(para == "IGD")
		{ 
			GetComparedMetric(format, res.igd_mean, res.igd_std, res.igd_median, res.igd_iqr, metric1, metric2);
		}
		else if(para == "Runtime")
		{
			GetComparedMetric(format, res.runtime_mean, res.runtime_std, res.runtime_median, res.runtime_iqr, metric1, metric2);
		}
		else if(para == "HV")
		{
			GetComparedMetric(format, res.hv_mean, res.hv_std, res.hv_median, res.hv_iqr, metric1, metric2);
		}
		else
		{
			// TODO
		}
	}

	void ExperimentTable::GetComparedMetric(const std::string& format, double mean, double std, double median, double iqr, double& metric1, double& metric2)
	{
		if (format == "Mean" || format == "Mean(STD)")
		{
			metric1 = mean;
			metric2 = std;
		}
		else if (format == "Median" || format == "Median(IQR)")
		{
			metric1 = median;
			metric2 = iqr;
		}
	}

}
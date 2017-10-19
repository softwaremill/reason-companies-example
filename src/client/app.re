open Company;
open Employee;

type action =
  | CompaniesLoaded (list company)
  | EmployeesLoaded (list employee) string
  | SearchResultsLoaded (list string)
  | QueryUpdated string
;

type componentState = {
	companies: list company,
	employees: list employee,
	selectedCompany: string,
	query: string,
	results: list string
};
let component = ReasonReact.reducerComponent "App";
let selectCompany = fun reduce (companyName: string) => {
	Js.log(companyName);
	CompanyService.fetchEmployees(companyName) |> Js.Promise.then_ (fun employees => {
		reduce (fun _ => (EmployeesLoaded employees companyName)) ();
		Js.Promise.resolve ();
	}) |> ignore;
};
let loadCompanies = fun reduce => {
	CompanyService.fetchCompanies() |> Js.Promise.then_ (fun companies => {
		reduce (fun _ => CompaniesLoaded companies) ();
		Js.Promise.resolve ();
	}) |> ignore;
};
let updateQuery = fun reduce query => {
	reduce (fun _ => QueryUpdated query) ();
};
let executeQuery = fun reduce query => {
	CompanyService.fetchSearch (query) |> Js.Promise.then_ (fun results => {
		reduce (fun _ => SearchResultsLoaded results) ();
		Js.Promise.resolve ();
	}) |> ignore;
};
let make _children => {
	...component,
	initialState: fun (): componentState => {
		companies: [],
		employees: [],
		selectedCompany: "",
		query: "",
		results: []
	},
	didMount: fun {reduce} => {
		loadCompanies reduce;
		ReasonReact.NoUpdate;
	},
	reducer: fun action state  => {
		switch action {
			| CompaniesLoaded companies => ReasonReact.Update { ...state, companies: companies }
			| EmployeesLoaded employees companyName => ReasonReact.Update {
				 ...state, employees: employees, selectedCompany: companyName, results: [], query: ""
			  }
			| QueryUpdated query => ReasonReact.Update { ...state, query: query }
			| SearchResultsLoaded results => ReasonReact.Update { ...state, results: results }
		}
	},
	render: fun {state, reduce} => {
		<div>
			<Header
				query=state.query
				onQueryUpdate=(fun event => {
					let query = (ReactDOMRe.domElementToObj (ReactEventRe.Form.target event))##value;
					updateQuery reduce query;
					executeQuery reduce query;
				})
				onQueryExecute=(fun _ => executeQuery reduce state.query)/>
			(state.query != ""
			? <SearchResults results=state.results/>
			: <CompanyList
				companies=state.companies
				employees=state.employees
				selectedCompany=state.selectedCompany
				onSelectCompany=(fun name => selectCompany reduce name)/>
			)
		</div>
	}
};

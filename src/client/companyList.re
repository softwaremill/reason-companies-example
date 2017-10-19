[%%bs.raw {| require('./companyList.css') |}];

open Utils;
open Company;
open Employee;

let component = ReasonReact.statelessComponent "CompanyList";

let make
	companies::(companies: (list company))
	employees::(employees: (list employee))
	selectedCompany::(selectedCompany: string)
	onSelectCompany::(onSelectCompany: string => unit) _children => {
	...component,
	render: fun _self => {
		<div className="row gutters main">
			<div className="col col-3">
				<ul className="unstyled">
					(listToElement (
						companies |> List.map (fun c => 
							<li key=c.name 
								className=(c.name == selectedCompany ? "active" : "")
								onClick=(fun _event => onSelectCompany c.name)>
								(str c.name)
							</li>
						)
					))
				</ul>
			</div>
			<div className="col col-9">
				<table className="bordered">
					<thead>
						<tr>
							<th>(str "First name")</th>
							<th>(str "Last name")</th>
							<th>(str "Birthday")</th>
							<th>(str "Salary")</th>
						</tr>
					</thead>
					<tbody>
						(listToElement (
							employees |> List.map (fun e => 
								<tr key=e.lastName>
									<td> {str e.firstName} </td>
									<td> {str e.lastName} </td>
									<td> {str e.birthday} </td>
									<td> {str ("$" ^ string_of_float e.salary)} </td>
								</tr>
							)
						))					
					</tbody>
				</table>
			</div>
		</div>
	}
}

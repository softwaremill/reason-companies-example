open Utils;

let component = ReasonReact.statelessComponent("SearchResults");

let make = (~results: list(string), _children) => {
	...component,
	render: _self => {
		<div className="row gutters main">
			<div className="col col-12">
				<table className="bordered">
					<tbody>
						(listToElement(
							results |> List.mapi((idx, result) => 
								<tr key=(string_of_int(idx))>
									<td> {str(result)} </td>
								</tr>
							)
						))
					</tbody>
				</table>
			</div>
		</div>
	}
}

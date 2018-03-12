open Company;
open Employee;

let parseCompany = (json: Js.Json.t): company => {
	name: Json.Decode.string(json),
	employees: []
};

let parseCompanies = (json: Js.Json.t) => {
	let companies = Json.Decode.array(parseCompany, json);
	Array.to_list(companies);
};

let parseEmployee = (json: Js.Json.t) => {
	firstName: json |> Json.Decode.field("firstName", Json.Decode.string),
	lastName: json |> Json.Decode.field("lastName", Json.Decode.string),
	birthday: json |> Json.Decode.field("birthday", Json.Decode.string),
	salary: json |> Json.Decode.field("salary", Json.Decode.float)
};

let parseEmployees = (json: Js.Json.t) => {
	let employees = Json.Decode.array(parseEmployee, json);
	Array.to_list(employees);
};

let parseSearch = (json: Js.Json.t) => {
	let results = Json.Decode.array(Json.Decode.string, json);
	Array.to_list(results);
};

let apiUrl = "http://localhost:3000";

let fetchCompanies = () => {
	Fetch.fetch(apiUrl ++ "/companies")
	|> Js.Promise.then_(Fetch.Response.json)
	|> Js.Promise.then_ ((json: Js.Json.t) => {
		let companies = parseCompanies(json);
		Js.Promise.resolve(companies);
	});
};

let fetchEmployees = (name: string) => {
	Fetch.fetch(apiUrl ++ "/employees?name=" ++ name)
	|> Js.Promise.then_(Fetch.Response.json)
	|> Js.Promise.then_ ((json: Js.Json.t) => {
		let employees = parseEmployees(json);
		Js.Promise.resolve(employees);
	});
};

let fetchSearch = (query: string) => {
	Fetch.fetch (apiUrl ++ "/search?query=" ++ query)
	|> Js.Promise.then_(Fetch.Response.json)
	|> Js.Promise.then_ ((json: Js.Json.t) => {
		let results = parseSearch(json);
		Js.Promise.resolve(results);
	});
};

open Express;
open Utils;
open Company;
open Employee;

let app = express();

[%%bs.raw{|
var bodyParser = require('body-parser');
app.use(bodyParser.json());
|}];

let companies: ref (list company) = ref [
	{ name: "SoftwareMill", employees: [
		{ firstName: "Daniel", lastName: "Kos", birthday: "1979-08-16", salary: 1000.0 },
		{ firstName: "Zenon", lastName: "Gamonski", birthday: "1943-06-16", salary: 1200.0 }		
	] },
	{ name: "UnsafeCode", employees: [
		{ firstName: "Jack", lastName: "Strong", birthday: "1987-01-15", salary: 5000.0 },
		{ firstName: "Jack", lastName: "Weak", birthday: "1983-05-10", salary: 6000.0 }		
	] }
];
let __dirname: option string = [%bs.node __dirname];
print_endline ("Current directory: " ^ (getOpt(__dirname, "-")));

module Response_ = {
	include Response;
	external setHeader: t => string => string => unit = "" [@@bs.send];
};

let employeeToJson = fun (e: employee) => {
	let json = Js_dict.empty();
	Js_dict.set json "firstName" (Js_json.string e.firstName);
	Js_dict.set json "lastName" (Js_json.string e.lastName);
	Js_dict.set json "birthday" (Js_json.string e.birthday);
	Js_dict.set json "salary" (Js_json.number e.salary);
	json;
};

let companyToJson = fun (c: company) => {
	let jsonEmployees = Array.of_list c.employees 
		|> Array.map (fun e => Js_json.object_ (employeeToJson e));
	let json = Js_dict.empty();
	Js_dict.set json "name" (Js_json.string c.name);
	Js_dict.set json "employees" (Js_json.array jsonEmployees);
	json;
};

let setRespHeaders = fun res => {
	Response_.setHeader res "Content-Type" "application/json";
	Response_.setHeader res "Access-Control-Allow-Origin" "*";
    Response_.setHeader res "Access-Control-Allow-Methods" "GET, POST, OPTIONS, PUT, PATCH, DELETE";
    Response_.setHeader res "Access-Control-Allow-Headers" "X-Requested-With,content-type";
};

App.get app path::"/companies" @@ Middleware.from (fun req res _ => {
	let name = getDictString (Request.query req) "name";
	let firstName = getDictString (Request.query req) "firstName";
	let lastName = getDictString (Request.query req) "lastName";
	let birthday = getDictString (Request.query req) "birthday";
	let salary = getDictNumber (Request.query req) "salary";

	let hasEmp = fun (emps: list employee, p: employee => bool) => 
		List.length emps == 0 || List.exists p emps;

	let jsonCompanies = !companies
		|> List.filter (fun c => c.name == getOpt(name, c.name))
		|> List.filter (fun c => hasEmp (c.employees, fun e => e.firstName == getOpt(firstName, e.firstName)))
		|> List.filter (fun c => hasEmp (c.employees, fun e => e.lastName == getOpt(lastName, e.lastName)))
		|> List.filter (fun c => hasEmp (c.employees, fun e => e.birthday == getOpt(birthday, e.birthday)))
		|> List.filter (fun c => hasEmp (c.employees, fun e => e.salary == getOpt(salary, e.salary)))
		|> Array.of_list
		|> Array.map (fun c => Js_json.object_ (companyToJson c));
	
	setRespHeaders res;
	Response_.sendJson res (Js_json.array jsonCompanies);
});

App.post app path::"/companies" @@ Middleware.from (fun req res _ => {
	let name = getDictString (Request.params req) "name";
	let defaultName = "Company " ^ string_of_int (List.length !companies);
	companies := [{name: getOpt(name, defaultName), employees: []}] @ !companies;
	Response.sendJson res (makeSuccessJson())
});

App.post app path::"/employees" @@ Middleware.from (fun req res _ => {
	let reqData = Request.asJsonObject req;
	let body = getDictObject reqData "body";
	let getBody = fun key => switch (body) {
	| Some json => getDictString json key
	| _ => None
	};
	let name = getOpt(getBody("name"), "");
	let firstName = getOpt(getBody("firstName"), "");
	let lastName = getOpt(getBody("lastName"), "");
	let birthday = getOpt(getBody("birthday"), "");
	let salary = getOpt(getBody("salary"), "0.0");

	let company = List.find (fun c => c.name == name) !companies;
	companies := List.map(fun c => c.name == name ? {
		name: company.name,
		employees: company.employees @ [{
			firstName: firstName,
			lastName: lastName,
			birthday: birthday,
			salary: float_of_string salary
		}]
	} : c) !companies;
	Response.sendJson res (makeSuccessJson());
});

App.useOnPath app path::"__dirname" {
    let options = Static.defaultOptions ();
    Static.make "static" options |> Static.asMiddleware
};

App.listen app port::3000;

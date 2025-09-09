// App.js


const { useState, useEffect } = React;

function App() {
	// Dynamic sections array
	const [sections, setSections] = useState(["Door", "Game", "Theatre"]);
	const [activeTab, setActiveTab] = useState("Global Commands");
	const [pixelState, setPixelState] = useState({});
	const [loading, setLoading] = useState(false);
	const [error, setError] = useState(false);

	useEffect(() => {
		console.log("App " + name + " loading", loading);

	}, [loading]);

	useEffect(() => {
		console.log("fetch pixelState");
		setLoading(true);

		fetch("/api")
			.then((response) => {
				if (!response.ok) {
					throw new Error("Network response was not ok");
				}
				return response.json();
			})
			.then((json) => {
				if (json.status != 200) {
					throw new Error("Json response was not ok");
				}
				console.log("got pixelState", json);
				setPixelState(json.result);

				const loadOff = async () => {
					await new Promise((resolve) => setTimeout(resolve, 3000));
					setLoading(false);
				};
				loadOff();
			})
			.catch((err) => {
				setError(err);
				setLoading(false);
			});
	}, []); // empty deps = run once after first render

	return (
		<div>
			<Navbar
				sections={sections}
				activeTab={activeTab}
				onTabChange={setActiveTab}
				loading={loading}
			/>
			<div className="container mt-3">
				{activeTab === "Global Commands" ? (
					<GlobalCommands loading={loading} pixelState={pixelState} />
				) : (
					<Section name={activeTab} loading={loading} pixelState={pixelState} />
				)}
			</div>
		</div>
	);
}

function Navbar({ sections, activeTab, onTabChange, loading, pixelState }) {
	useEffect(() => {
		console.log("Navbar loading", loading);

	}, [loading]);

	return (
		<nav className="navbar navbar-expand-lg bg-body-tertiary">
			<div className="container-fluid">
				<a className="navbar-brand" href="#">Paul's Funky Lighting 2.0</a>
				<button className="navbar-toggler" type="button"
					data-bs-toggle="collapse" data-bs-target="#navbarNavAltMarkup">
					<span className="navbar-toggler-icon"></span>
				</button>
				<div className="collapse navbar-collapse" id="navbarNavAltMarkup">
					<ul className="navbar-nav me-auto mb-2 mb-lg-0">
						<li className="nav-item">
							<button
								className={`nav-link btn ${activeTab === "Global Commands" ? "active" : ""}`}
								style={{ border: "none", background: "none" }}
								onClick={() => onTabChange("Global Commands")}
							>
								Overview
							</button>
						</li>
						{sections.map((section) => (
							<li className="nav-item" key={section}>
								<button
									className={`nav-link btn ${activeTab === section ? "active" : ""}`}
									style={{ border: "none", background: "none" }}
									onClick={() => onTabChange(section)}
								>
									{section}
								</button>
							</li>
						))}
					</ul>
				</div>

			</div>
		</nav>

	);
}

function GlobalCommands({ loading, pixelState }) {
	useEffect(() => {
		console.log("GlobalCommands loading", loading);

	}, [loading]);
	return (
		<div>
			<h3 className="d-flex justify-content-between align-items-center">
				Overview
				<button className="btn btn-sm btn-outline-secondary" disabled={loading}>
					<i className="bi bi-arrow-clockwise"></i>
					{loading}
				</button>
			</h3>

			<p>Commands that affect everything.</p>
			<pre>{JSON.stringify(pixelState, (k, v) => k.length > 0 && k[0] === k[0].toLocaleUpperCase() ? undefined : v, 2)}</pre>

		</div>
	);
}

function Section({ name, loading, pixelState }) {
	const [section, setSection] = useState({});


	useEffect(() => {
		console.log("Section " + name + " changed", name);
		setSection(pixelState[name]);

	}, [name]);
	useEffect(() => {
		console.log("Section " + name + " loading", loading);
		setSection(pixelState[name]);

	}, [loading]);
	return (
		<div>
			<h3 className="d-flex justify-content-between align-items-center">
				{name}
				<button className="btn btn-sm btn-outline-secondary" disabled={loading}>
					<i className="bi bi-arrow-clockwise"></i>
				</button>
			</h3>
			<p>Controls for {name}.</p>
			<pre>{JSON.stringify(section, null, 2)}</pre>
		</div>
	);
}


// Important: expose App to the global scope
window.App = App;

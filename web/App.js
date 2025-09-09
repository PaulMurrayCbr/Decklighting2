// App.js
//function App() {
//  return (
//    <div className="container text-center mt-5">
//      <h1 className="text-primary">Paul's Funky Lighting 2.0</h1>
//      <button className="btn btn-success">Click Me</button>
//    </div>
//  );
//}


const { useState } = React;

function App() {
	// Dynamic sections array
	const [sections, setSections] = useState(["Section 1", "Section 2", "Section 3"]);
	const [activeTab, setActiveTab] = useState("Global Commands");

	return (
		<div>
			<Navbar
				sections={sections}
				activeTab={activeTab}
				onTabChange={setActiveTab}
			/>
			<div className="container mt-3">
				{activeTab === "Global Commands" ? (
					<GlobalCommands />
				) : (
					<Section name={activeTab} />
				)}
			</div>
		</div>
	);
}

function Navbar({ sections, activeTab, onTabChange }) {
	return (
		<nav class="navbar navbar-expand-lg bg-body-tertiary">
			<div class="container-fluid">
				<a class="navbar-brand" href="#">Paul's Funky Lighting 2.0</a>
				<button class="navbar-toggler" type="button"
					data-bs-toggle="collapse" data-bs-target="#navbarNavAltMarkup">
					<span class="navbar-toggler-icon"></span>
				</button>
				<div class="collapse navbar-collapse" id="navbarNavAltMarkup">
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

function GlobalCommands() {
	return (
		<div>
		<h3 class="d-flex justify-content-between align-items-center">
		  Overview
		  <button class="btn btn-sm btn-outline-secondary">
		    <i class="bi bi-arrow-clockwise"></i>
		  </button>
		</h3>			

			<p>Commands that affect everything.</p>
		</div>
	);
}

function Section({ name }) {
	return (
		<div>
		<h3 class="d-flex justify-content-between align-items-center">
		  {name}
		  <button class="btn btn-sm btn-outline-secondary">
		    <i class="bi bi-arrow-clockwise"></i>
		  </button>
		</h3>			
			<p>Controls for {name}.</p>
		</div>
	);
}


// Important: expose App to the global scope
window.App = App;

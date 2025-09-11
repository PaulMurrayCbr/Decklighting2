
function SectionPage({ name, loading, pixelState, info, apiSection, home }) {
	const [section, setSection] = useState({});

	const reload = () => {
		apiSection(name, '');
	};

	useEffect(() => {
		setSection(pixelState[name]);
	}, [name, pixelState]);

	return (
		<div>
			<div className="d-flex justify-content-between align-items-center flex-wrap mb-3">
				<nav aria-label="breadcrumb">
					<ol className="breadcrumb">
						<li className="breadcrumb-item">
							<a href="#" onClick={home}><i className="fa-solid fa-house"></i> Home</a>
						</li>
						<li className="breadcrumb-item active" aria-current="page">{name}</li>
					</ol>
				</nav>
				<button className="btn btn-outline-secondary btn-sm" disabled={loading} onClick={reload}>
					<i className="fa-solid fa-rotate"></i>
				</button>
			</div>

			<pre>{JSON.stringify(section, null, 2)}</pre>
		</div>
	);
}

window.SectionPage = SectionPage;
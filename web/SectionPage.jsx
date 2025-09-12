
const { useState, useEffect, useRef } = React;


function SectionPage({ name, loading, pixelState, info, apiSection, home }) {

	const [section, setSection] = useState({});
	const [selected, setSelected] = useState({ main: true, color: 0 });
	const [nColors, setNColors] = useState(0);

	useEffect(() => {
		setSection(pixelState[name] ?? {});
	}, [pixelState, name])

	useEffect(() => {
		const n = info?.effectInfo[section?.effect]?.ncolorranges ?? 0;
		setNColors(n);

		if (!selected.main && selected.color >= n) {
			selectMainPage();
		}
	}, [section, info])


	function reload() {
		apiSection(name, '');
	};

	function selectMainPage() {
		setSelected({
			...selected,
			main: true
		});
	};

	function selectColor(c) {
		setSelected({
			...selected,
			main: false,
			color: c
		});
	};

	function mainPillActive() {
		return selected.main ? 'active' : '';
	}

	function colorPillActive(i) {
		return !selected.main && selected.color === i ? 'active' : '';
	}

	function colorPillName(i) {
		return `Color${nColors < 2 ? '' : ' ' + String.fromCharCode(65 + i)}`
	}

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

			<ul className="nav nav-pills mb-3">
				<li className="nav-item">
					<button className={`nav-link ${mainPillActive()}`} onClick={selectMainPage} >Main</button>
				</li>

				{Array.from({ length: nColors }).map((_, i) => (
					<li className="nav-item" key={"color-pick-pill-" + i}>
						<button className={`nav-link ${colorPillActive(i)}`}
							onClick={() => selectColor(i)}
						>{colorPillName(i)}</button>
					</li>
				))}
			</ul>

			{selected.main && (
				<SectionMain
					section={section}
					info={info}
					apiSection={(url) => apiSection(name, url)}
				/>
			)}
			{!selected.main && (
				<SectionColor
					color={section.color[selected.color]}
					info={info}
					apiColor={(url) => apiSection(name, `color/${selected.color}${url}`)}
				/>
			)}

		</div>
	);
}

class Debouncer {
	debounce = undefined;
	ms = 300;
	setMethod = () => { }
	apiCall = () => { }

	constructor(ms, setMethod, apiCall) {
		this.ms = ms;
		this.setMethod = setMethod;
		this.apiCall = apiCall;
	}

	set(value) {
		this.setMethod(value);
		if (this.debounce) clearTimeout(this.debounce);
		this.debounce = setTimeout(() => {
			this.apiCall(value);
			this.debounce = undefined;
		}, this.ms);
	}
}

function SectionMain({ section, info, apiSection }) {

	const togglePower = () => {
		if (section?.mode === "on") {
			apiSection("off");
		}
		else {
			apiSection("on");
		}
	}

	const [densityState, setDensityState] = useState(0);
	const densityDebouncer = useRef(new Debouncer(300, setDensityState,
		(value) => apiSection(`set?density=${value}`)
	));


	const activeEffectPill = useRef(null);

	useEffect(() => {
		setDensityState(section?.density ?? 1);

		if (activeEffectPill.current) {
			activeEffectPill.current.scrollIntoView({ behavior: "smooth", inline: "center" });
		}
	}, [section]);

	function effectPillActive(name) {
		return section?.effect === name ? 'active' : '';
	}

	function setEffect(value) {
		apiSection(`set?effect=${value}`);
	}

	return (
		<div>
			<div className="d-flex justify-content-around align-items-center flex-wrap mb-3">
				<button
					className={`btn btn-outline-${section?.mode === "on" ? "success" : "danger"} btn-md`}
					onClick={togglePower}
					style={{ width: "100px", height: "100px", borderRadius: "50%" }}
				>
					<i className="fa-solid fa-power-off fa-2x"></i>
				</button>
			</div>



			<div className="d-flex align-items-center ">
				<input
					type="range"
					className="form-range"
					min="1"
					max="10"
					value={densityState}
					onChange={(e) => densityDebouncer.current.set(Number(e.target.value))}
				/>
			</div>

			<div className="d-flex justify-content-between w-100 mb-4">

				{Array.from({ length: 11 }).map((_, i) => (
					<i
						key={`bulb-${i}`}
						className="fa-solid fa-lightbulb"
						style={{ color: i % densityState == 0 ? 'gold' : 'darkgray' }}></i>
				))}

			</div>

			<div className="overflow-auto scrollable-pills">
				<ul className="nav nav-pills flex-nowrap">

					{info?.effects?.map((ename) => (
						<li key={`effect-pill-${ename}`}
							className="nav-item">
							<button className={`nav-link ${effectPillActive(ename)}`}
								onClick={() => setEffect(ename)}
								ref={section?.effect === ename ? activeEffectPill : null}
							>{ename}</button>
						</li>
					))}
				</ul>
			</div>
		</div>

	);
}


function toRgb(value) {
	// Remove leading #
	value = value.replace(/^#/, '');

	if (value.length !== 6) {
		console.log(`weird color value ${value}`)
		return;
	}

	const r = parseInt(value.slice(0, 2), 16);
	const g = parseInt(value.slice(2, 4), 16);
	const b = parseInt(value.slice(4, 6), 16);

	return `r=${r}&g=${g}&b=${b}`;

}

function SectionColor({ color, info, apiColor }) {


	const [fromColorState, setFromColorState] = useState(0);
	const fromColorDebouncer = useRef(new Debouncer(300, setFromColorState,
		(value) => apiColor(`/from?${toRgb(value)}`)
	));

	const [toColorState, setToColorState] = useState(0);
	const toColorDebouncer = useRef(new Debouncer(300, setToColorState,
		(value) => apiColor(`/to?${toRgb(value)}`)
	));

	const [biasState, setBiasState] = useState(500);
	const biasDebouncer = useRef(new Debouncer(300, setBiasState,
		(value) => apiColor(`?bias=${value / 10000}`)
	));


	const activeInterpolationPill = useRef(null);

	function interpolationPillActive(name) {
		return color?.interpolation === name ? 'active' : '';
	}


	useEffect(() => {
		setFromColorState(color?.from ?? '#000000');
		setToColorState(color?.to ?? '#000000');
		setBiasState(color?.bias * 10000 ?? 500);
		if (activeInterpolationPill.current) {
			activeInterpolationPill.current.scrollIntoView({ behavior: "smooth", inline: "center" });
		}

	}, [color])

	return (
		<div>
			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">From/To</label>
				<div className="col-sm-9">
					<input
						type="color"
						className="form-control-color"
						value={fromColorState}
						onChange={e => fromColorDebouncer.current.set(e.target.value)}
					/>
					<input
						type="color"
						className="form-control-color"
						value={toColorState}
						onChange={e => toColorDebouncer.current.set(e.target.value)}
					/>
				</div>
			</div>

			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">Transition</label>
				<div className="col-sm-9">



					<div className="overflow-auto scrollable-pills">
						<ul className="nav nav-pills flex-nowrap">
							{info?.interpolations?.map((iname) => (
								<li key={`interpolation-pill-${iname}`}
									className="nav-item">
									<button className={`nav-link ${interpolationPillActive(iname)}`}
										onClick={() => apiColor(`?interpolation=${iname}`)}
										ref={color?.interpolation === iname ? activeInterpolationPill : null}
									>{iname}</button>
								</li>
							))}
						</ul>
					</div>


				</div>
			</div>

			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">Seamless</label>
				<div className="col-sm-9">
					<input
						type="checkbox"
						className="form-check-input"
						checked={color.seamless}
						onChange={e => {
							const value = e.target.checked;
							apiColor(`?seamless=${value}`)
						}}
					/>
				</div>
			</div>

			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">Bias</label>
				<div className="col-sm-9">
					<input
						type="range"
						className="form-range"
						min="1"
						max="9999"
						value={biasState}
						onChange={e => biasDebouncer.current.set(parseFloat(e.target.value))}
					/>
				</div>
			</div>

			<hr />

			<div className="row ">
				<label className="col-sm-3 col-form-label">Animating</label>
				<div className="col-sm-9">
					<input
						type="checkbox"
						className="form-check-input"
						checked={color.animating}
						onChange={e => {
							const value = e.target.checked;
							apiColor(`?animating=${value}`)
						}}
					/>
				</div>
			</div>



			<pre>
				{JSON.stringify(color, null, 2)}
			</pre>
		</div>

	);
}

window.SectionPage = SectionPage;